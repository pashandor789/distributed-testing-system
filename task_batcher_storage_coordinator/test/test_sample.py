import json
import string
import sys

sys.path.append('proto')

import grpc
import proto.tabasco_grpc_pb2
import proto.tabasco_grpc_pb2_grpc

import copy
import random
import requests

import pytest

HTTP_TABASCO_URL = 'http://http_tabasco:8080'
GRPC_TABASCO_URL = 'grpc_tabasco:9090'


def post_request(url, **kwargs):
    return noexcept_request(url, requests.post, **kwargs)


def get_request(url, **kwargs):
    return noexcept_request(url, requests.get, **kwargs)


def put_request(url, **kwargs):
    return noexcept_request(url, requests.put, **kwargs)


def noexcept_request(url, method, data=None, files=None, json=None):
    response = None

    try:
        response = method(
            url=url,
            data=data,
            files=files,
            json=json,
            timeout=20
        )
    except requests.exceptions.ConnectionError:
        pass

    assert response is not None, 'Tabasco connection failed! More likely it has not been launched yet.'

    return response


@pytest.fixture(scope="module")
def uploaded_test_a_plus_b():
    tests = {}

    for i in range(1, 30 + 1):
        a, b = random.randint(-1_000_000, 1_000_000), random.randint(-1_000_000, 1_000_000)
        tests[f'{i}_input'] = f'{a} {b}'
        tests[f'{i}_output'] = f'{a + b}'

    return tests


@pytest.fixture(scope="module")
def uploaded_test_big_string():
    tests = {}

    for i in range(1, 5 + 1):
        big_string = ''.join([random.choice(string.ascii_letters) for _ in range(10_000_000)])
        tests[f'{i}_input'] = big_string
        tests[f'{i}_output'] = big_string

    return tests


@pytest.fixture(scope="module")
def change_this_build_name():
    return f'changeThis{random.randint(-1_000_000, 1_000_000)}'


@pytest.fixture(scope="module")
def build_name():
    return f'testBuild{random.randint(-1_000_000, 1_000_000)}'


init_script = '''mv $1 main.cpp; g++ main.cpp -o executable'''
execute_script = './executable'


def upload_tests(task_id, tests):
    files = copy.deepcopy(tests)
    files['taskId'] = task_id
    response = put_request(f'{HTTP_TABASCO_URL}/uploadTests', files=files)

    assert response.status_code == 200, f'uploadTest failed: {response.content.decode()}'


def create_build(data):
    response = post_request(f'{HTTP_TABASCO_URL}/createBuild', json=data)
    assert response.status_code == 200, f'createBuild failed: {response.content.decode()}'


class TestHTTPTabasco:
    def test_create_build_handler(self, build_name, change_this_build_name):
        data = {
            'buildName': build_name,
            'description': 'test description',
            'executeScript': execute_script,
            'initScript': init_script
        }

        create_build(data)

    def test_builds_handler(self):
        response = get_request(f'{HTTP_TABASCO_URL}/builds')
        builds = json.loads(response.content.decode())["items"]

        assert response.status_code == 200, f'builds failed: {response.content.decode()}'

        filter_func = lambda x: (
                x['executeScript'] == execute_script and
                x['initScript'] == init_script
        )

        filtered_builds = list(filter(filter_func, builds))

        assert len(filtered_builds) > 0, f'build was not found in {filtered_builds}'

        assert filtered_builds[0]['executeScript'] == execute_script
        assert filtered_builds[0]['initScript'] == init_script

    def test_update_build_handler(self, change_this_build_name):
        data = {
            'buildName': change_this_build_name,
            'description': 'test description',
            'executeScript': 'change this',
            'initScript': 'change this'
        }

        create_build(data)

        data['executeScript'] = 'changed'
        data['initScript'] = 'changed'

        response = put_request(f'{HTTP_TABASCO_URL}/updateBuild', json=data)
        assert response.status_code == 200, f'updateBuild failed: {response.content.decode()}'

        response = get_request(f'{HTTP_TABASCO_URL}/builds')
        builds = json.loads(response.content.decode())["items"]

        assert response.status_code == 200, f'builds failed: {response.content.decode()}'

        filter_func = lambda x: x['executeScript'] == 'changed' and x['initScript'] == 'changed' and x['name'] == change_this_build_name

        filtered_builds = list(filter(filter_func, builds))

        assert len(filtered_builds) > 0, f'build was not found in {filtered_builds}'

        filter_func = lambda x: (
                x['executeScript'] == 'change this' and
                x['initScript'] == 'change this'
        )

        filtered_builds = list(filter(filter_func, builds))

        assert len(filtered_builds) == 0

    def test_upload_tests_handler_small_tests(self, uploaded_test_a_plus_b):
        upload_tests(task_id=0, tests=uploaded_test_a_plus_b)

    def test_upload_tests_handler_big_tests(self, uploaded_test_big_string):
        upload_tests(task_id=1, tests=uploaded_test_big_string)


def get_grpc_tabasco_stub():
    channel = grpc.insecure_channel(
        GRPC_TABASCO_URL,
        options=[
            ('grpc.max_send_message_length', 150_000_000),
            ('grpc.max_receive_message_length', 150_000_000)
        ]
    )
    stub = proto.tabasco_grpc_pb2_grpc.TTabascoGRPCStub(channel)
    return stub


def get_script_and_get_batch(task_id, expected_tests, build_name):
    stub = get_grpc_tabasco_stub()

    request = proto.tabasco_grpc_pb2.TGetScriptsRequest(task_id=task_id, build_name=build_name)
    response = stub.GetScripts(request)

    assert response.init_script == init_script
    assert response.execute_script == execute_script
    assert response.batch_count > 0

    test_index = 1

    for batch_id in range(response.batch_count):
        request = proto.tabasco_grpc_pb2.TGetBatchRequest(task_id=task_id, batch_id=batch_id)
        response = stub.GetBatch(request)

        assert len(response.input) > 0
        assert len(response.input) == len(response.output)

        for input_test, output_test in zip(response.input, response.output):
            assert input_test == expected_tests[f'{test_index}_input']
            assert output_test == expected_tests[f'{test_index}_output']

            expected_tests.pop(f'{test_index}_input')
            expected_tests.pop(f'{test_index}_output')

            test_index += 1

    assert len(expected_tests) == 0


class TestGRPCTabasco:
    def test_get_script_and_get_batch_small_tests(self, uploaded_test_a_plus_b, build_name):
        get_script_and_get_batch(0, uploaded_test_a_plus_b, build_name)

    def test_get_script_and_get_batch_big_tests(self, uploaded_test_big_string, build_name):
        get_script_and_get_batch(1, uploaded_test_big_string, build_name)

# import pika
#
#
# def get_broker_channel():
#     connection = pika.BlockingConnection(pika.ConnectionParameters('broker'))
#     channel = connection.channel()
#     channel.queue_declare(queue='test')
#     return channel
#
#
# user_data_a_plus_b = '''
# #include <iostream>
#
# int main() {
#     int a, b;
#     std::cin >> a >> b;
#     std::cout << a + b;
# }
# '''
#
#
# class TestTestingProcessor:
#     def test_submit_small_test(self):
#         channel = get_broker_channel()
#
#         json_message = {
#             "submissionId": 0,
#             "buildId": 1,
#             "userData": user_data_a_plus_b,
#             "taskId": 0,
#             "memoryLimit": 10241024,
#             "cpuTimeLimitMilliSeconds": 2000
#         }
#
#         channel
