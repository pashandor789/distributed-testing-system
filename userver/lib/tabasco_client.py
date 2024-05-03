import requests
import environs
import json

from pydantic import BaseModel

env = environs.Env()
env.read_env()
HTTP_TABASCO_URL = env("HTTP_TABASCO_URL")

class Build(BaseModel):
    init_script: str
    execution_script: str
    description: str

def create_build(build_id: int, build: Build):
        data = {
            "id": build_id,
            "executeScript": build.execution_script,
            "initScript": build.init_script
        }
        url = f"{HTTP_TABASCO_URL}/uploadBuild"
        response = requests.put(
            url=url,
            json=data,
            timeout=20
        )
        if not response or response.status_code != 200:
            raise RuntimeError(response.content.decode())


class Test(BaseModel):
    input_data: str
    output_data: str


class Tests(BaseModel):
    tests: list[Test]

def upload_tests(task_id: int, tests: Tests):
    files = {}
    for i, test in enumerate(tests.tests, 1): 
        files[f"{i}_input"] = test.input_data
        files[f"{i}_output"] = test.output_data
    files["taskData.json"] = json.dumps({"taskId": task_id})
    url = f"{HTTP_TABASCO_URL}/uploadTests"
    response = requests.put(
        url=url,
        files=files,
        timeout=20
    )
    if not response or response.status_code != 200:
        raise RuntimeError(response.content.decode())
