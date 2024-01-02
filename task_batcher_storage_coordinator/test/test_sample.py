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


def post_request(url, data=None, files=None, json=None):
    response = None

    try:
        response = requests.post(
            url=url,
            data=data,
            files=files,
            json=json
        )
    except requests.exceptions.ConnectionError:
        pass

    assert response != None, 'Tabasco connection failed! More likely it has not been launched yet.'

    return response


@pytest.fixture(scope="module")
def uploaded_test():
    tests = {}

    for i in range(1, 30 + 1):
        a, b = random.randint(-1_000_000, 1_000_000), random.randint(-1_000_000, 1_000_000)
        tests[f'{i}_input'] = f'{a} {b}'
        tests[f'{i}_output'] = f'{a + b}'

    return tests


init_script = 'g++ $1 -o executable'
execute_script = './executable'


class TestHTTPTabasco:
    def test_upload_tests_handler(self, uploaded_test):
        files = copy.deepcopy(uploaded_test)
        files['taskId'] = 0
        response = post_request(f'{HTTP_TABASCO_URL}/uploadTests', files=files)

        assert response.status_code == 200, f'uploadTest failed: {response.content.decode()}'

    def test_upload_scripts(self):
        data = {
            'scriptName': 'testInit',
            'content': init_script
        }

        response = post_request(f'{HTTP_TABASCO_URL}/uploadInitScript', json=data)

        assert response.status_code == 200, f'uploadInitScript failed: {response.content.decode()}'

    def test_upload_execute_script(self):
        data = {
            'scriptName': 'testExecute',
            'content': execute_script
        }

        response = post_request(f'{HTTP_TABASCO_URL}/uploadExecuteScript', json=data)

        assert response.status_code == 200, f'uploadExecuteScript failed: {response.content.decode()}'

    def test_create_build(self):
        data = {
            'buildName': 'testBuild',
            'executeScriptId': 1,
            'initScriptId': 1
        }

        response = post_request(f'{HTTP_TABASCO_URL}/createBuild', json=data)

        assert response.status_code == 200, f'createBuild failed: {response.content.decode()}'


def get_grpc_tabasco_stub():
    channel = grpc.insecure_channel(GRPC_TABASCO_URL)
    stub = proto.tabasco_grpc_pb2_grpc.TTabascoGRPCStub(channel)
    return stub


class TestGRPCTabasco:
    def test_get_script_and_get_batch(self, uploaded_test):
        stub = get_grpc_tabasco_stub()

        request = proto.tabasco_grpc_pb2.TGetScriptsRequest(task_id=0, build_id=1)
        response = stub.GetScripts(request)

        assert response.init_script == init_script
        assert response.execute_script == execute_script
        assert response.batch_count > 0

        tests = uploaded_test
        test_index = 1

        for batch_id in range(response.batch_count):
            request = proto.tabasco_grpc_pb2.TGetBatchRequest(task_id=0, batch_id=batch_id)
            response = stub.GetBatch(request)

            assert len(response.input) > 0
            assert len(response.input) == len(response.output)

            for input_test, output_test in zip(response.input, response.output):
                assert input_test == tests[f'{test_index}_input']
                assert output_test == tests[f'{test_index}_output']

                tests.pop(f'{test_index}_input')
                tests.pop(f'{test_index}_output')

                test_index += 1

        assert len(tests) == 0
