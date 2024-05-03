import logging

from typing import Annotated


from fastapi import FastAPI, Depends, HTTPException, Request
from fastapi.responses import JSONResponse
from fastapi.security import OAuth2PasswordBearer, OAuth2PasswordRequestForm

from lib.authlib import create_token, get_user_id_from_token
from lib.db.auth import get_user, add_user
from lib.db.challenges import get_challenges, get_challenge_by_id, get_tasks_by_challenge
from lib.db.tasks import Task, create_task, get_tasks, get_task_by_id, create_build_for_task
from lib.db.solutions import save_solution, TestsResults, save_resolution, get_solution_by_id, get_my_solutions
from lib.tabasco_client import Build, create_build, Tests, upload_tests
from lib.testing_processor_client import send_solution

app = FastAPI()
oauth2_scheme = OAuth2PasswordBearer(tokenUrl="/signin", scheme_name="jwt")


@app.get("/health/")
def read_health():
    return {"status" : "OK"}


@app.post("/signup")
def signup(username: str, password: str):
    try:
        add_user(username, password)
    except RuntimeError as exception:
        logging.warning(*exception.args)
        raise HTTPException(400, exception.args)

@app.post("/signin")
async def signin(form: Annotated[OAuth2PasswordRequestForm, Depends()]):
    try:
        username = form.username
        password = form.password
        user = get_user(username)
        print(user.__dict__)
        if str(hash(username + password)) != user.password:
            raise HTTPException(401, "wrong password")
        return  JSONResponse(
            {
                "access_token": create_token(user.id),
                "token_type": "Bearer"
            })
    except RuntimeError as exception:
        logging.warning(*exception.args)
        raise HTTPException(400, exception.args)   


@app.get("/challenges")
def get_all_challenges():
    return get_challenges()

@app.get("/tasks")
def get_all_tasks():
    return get_tasks()


@app.get("/challenges/{id}")
def get_challenge(id: int):
    challenge = get_challenge_by_id(id)
    if len(challenge) == 0:
        raise HTTPException(400, "Challenge doesn't exist")
    challenge = challenge[0]
    tasks = get_tasks_by_challenge(id)
    return {
        "name": challenge.name,
        "tasks": tasks
    }


@app.get("/tasks/{id}")
def get_task(id: int):
    try:
        return get_task_by_id(id)
    except RuntimeError as exception:
        logging.warning(*exception.args)
        raise HTTPException(400, exception.args)


@app.post("/tasks/{id}")
def post_solution(id: int, solution: str, build_id: int, token: Annotated[str, Depends(oauth2_scheme)]):
    try:
        task = get_task_by_id(id, True)
        solution_id = save_solution(solution, get_user_id_from_token(token), id)
        send_solution(solution, solution_id, build_id, task["id"], task["memory_limit"], task["time_limit"])
    except RuntimeError as exception:
        logging.warning(*exception.args)
        raise HTTPException(400, exception.args)

@app.post("/solutions/")
def commit_solution_result(results: TestsResults):
    try:
        save_resolution(results)
    except RuntimeError as exception:
        logging.warning(*exception.args)
        raise HTTPException(400, exception.args)

@app.get("/solutions/{id}")
def get_result_for_solution(id: int, token: Annotated[str, Depends(oauth2_scheme)]):
    try:
        return get_solution_by_id(id, get_user_id_from_token(token))
    except RuntimeError as exception:
        logging.warning(*exception.args)
        raise HTTPException(400, exception.args)


@app.get("/solutions")
def get_my_solution(token: Annotated[str, Depends(oauth2_scheme)]):
    try:
        return get_my_solutions(get_user_id_from_token(token))
    except RuntimeError as exception:
        logging.warning(*exception.args)
        raise HTTPException(400, exception.args)


@app.post("/admin/challenge")
def add_challenge():
    pass


@app.post("/admin/task")
def add_task(task: Task):
    try:
        create_task(task)
    except RuntimeError as exception:
        logging.warning(*exception.args)
        raise HTTPException(400, exception.args)


@app.post("/admin/task/{task_id}/build")
def add_build_for_task(task_id: int, build: Build):
    try:
        build_id = create_build_for_task(task_id, build.description)
        create_build(build_id, build)
    except RuntimeError as exception:
        logging.warning(*exception.args)
        raise HTTPException(400, exception.args)


@app.post("/admin/task/{task_id}/tests")
def add_tests_for_task(task_id: int, tests: Tests):
    try:
        upload_tests(task_id, tests)
    except RuntimeError as exception:
        logging.warning(*exception.args)
        raise HTTPException(400, exception.args)


@app.post("/admin/challenge/{challenge_id}")
def add_task_for_challenge(challenge_id: int):
    pass
