from sqlalchemy import Column, Integer, String, ForeignKey
from pydantic import BaseModel

from .helpers import delete_techincal_info
from .session import get_database, get_engine, Base

class SolutionDB(Base):
    __tablename__ = "solutions"

    id = Column(Integer, primary_key=True, index=True)
    user_id = Column(Integer, ForeignKey("users.id"))
    task_id = Column(Integer, ForeignKey("tasks.id"))
    text = Column(String)
    result = Column(String)

class TestResultDB(Base):
    __tablename__ = "test_results"

    id = Column(Integer, primary_key=True, index=True)
    solution_id = Column(Integer, ForeignKey("solutions.id"))
    cpu_time = Column(Integer)
    memory_used = Column(Integer)
    verdict = Column(String)


class Solution(BaseModel):
    text: str
    user_id: int
    task_id: int

Base.metadata.create_all(bind=get_engine())

def save_solution(text: str, user_id: int, task_id: int):
    db = get_database()
    solution = SolutionDB(text=text, user_id=user_id, task_id=task_id)
    db.add(solution)
    db.commit()
    db.refresh(solution)
    return solution.id

def get_my_solutions(user_id: int):
    db = get_database()
    return db.query(SolutionDB).filter(SolutionDB.user_id == user_id).all()

def get_solution_by_id(solution_id: int, user_id: int):
    db = get_database()
    solution = db.query(SolutionDB).filter(SolutionDB.id == solution_id).first() 
    if not solution:
        raise RuntimeError("solution doesn't exist")
    if solution.user_id != user_id:
        raise RecursionError("wrong id")
    tests = db.query(TestResultDB).filter(TestResultDB.solution_id == solution_id).all()
    solution = delete_techincal_info(solution, ["id"])
    solution["tests"] = []
    for test in tests:
        solution["tests"].append(delete_techincal_info(test, ["id", "solution_id"]))
    return solution

class TestResult(BaseModel):
    cpuTimeElapsedMilliSeconds: int
    memorySpent: int
    verdict: str

class TestsResults(BaseModel):
    report: list[TestResult]
    submissionId: int

def save_resolution(results: TestsResults):
    verdict = results.report[-1].verdict
    db = get_database()
    solution = db.query(SolutionDB).get(results.submissionId)
    if not solution:
        raise RuntimeError(f"not valid solution id: {results.submissionId}")
    solution.result = verdict
    db.add(solution)
    db.commit()
    test_results = list(map(
        lambda result: TestResultDB(
            solution_id=results.submissionId, 
            cpu_time=result.cpuTimeElapsedMilliSeconds,
            memory_used=result.memorySpent,
            verdict=result.verdict
        ),
        results.report
    ))
    db.add_all(test_results)
    db.commit()
