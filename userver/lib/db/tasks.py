from pydantic import BaseModel
from sqlalchemy import Column, Integer, String, ForeignKey

from typing import Optional

from .helpers import delete_techincal_info
from .session import get_database, get_engine, Base

class TaskDB(Base):
    __tablename__ = "tasks"
    
    id = Column(Integer, primary_key=True, index=True)
    name = Column(String, unique=True, index=True)
    condition = Column(String)
    time_limit = Column(Integer)
    memory_limit = Column(Integer)


class Task(BaseModel):
    name: str
    condition: str
    time_limit: int
    memory_limit: int


class BuildDB(Base):
    __tablename__ = "tasks_builds"

    id = Column(Integer, primary_key=True, index=True)
    task_id = Column(Integer, ForeignKey("tasks.id"))
    description = Column(String)


Base.metadata.create_all(bind=get_engine())

def create_task(task: Task):
    db = get_database()
    if len(db.query(TaskDB).filter(TaskDB.name == task.name).all()) > 0:
        raise RuntimeError(f"task with name {task.name} is already exist")
    db_task = TaskDB(**task.__dict__)
    db.add(db_task)
    db.commit()
    db.refresh(db_task)

def get_tasks():
    db = get_database()
    return db.query(TaskDB).all()

def get_task_by_id(id: int, without_builds=False):
    db = get_database()
    task = db.query(TaskDB).filter(TaskDB.id == id).first()
    if not task:
        raise RuntimeError("try to get invalid task")
    if without_builds:
        return task.__dict__
    result = delete_techincal_info(task, ["id"])
    builds = db.query(BuildDB).filter(BuildDB.task_id == id).all()
    result["builds"] = list(map(
        lambda build: delete_techincal_info(build, ["task_id"]),
        builds
    ))
    return result

def create_build_for_task(task_id: int, description: str):
    db = get_database()
    if db.query(TaskDB).filter(TaskDB.id == task_id).count() == 0:
        raise RuntimeError(f"task wasn't created, fix it before add build")
    build = {
        "task_id": task_id,
        "description": description
    }
    build = BuildDB(**build)
    db.add(build)
    db.commit()
    db.refresh(build)
    return build.id
