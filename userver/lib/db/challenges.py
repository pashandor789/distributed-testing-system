from sqlalchemy import Column, Integer, String, ForeignKey

from .session import get_database, get_engine, Base

from .tasks import TaskDB

class ChallengeDB(Base):
    __tablename__ = "challenges"
    
    id = Column(Integer, primary_key=True, index=True)
    name = Column(String, unique=True, index=True)


class ChallengeTaskDB(Base):
    __tablename__ = "challenges_tasks"

    id = Column(Integer, primary_key=True, index=True)
    challenge_id = Column(Integer, ForeignKey("challenges.id"))
    task_id = Column(Integer, ForeignKey("tasks.id"))


Base.metadata.create_all(bind=get_engine())


def get_challenges(offset: int = 0, limit: int = 100) -> list[ChallengeDB]:
    db = get_database()
    return db.query(ChallengeDB).offset(offset).limit(limit).all()


def get_challenge_by_id(id: int) -> list[ChallengeDB]:
    db = get_database()
    return db.query(ChallengeDB).filter(ChallengeDB.id == id).all()


def get_tasks_by_challenge(challenge_id: int) -> list[TaskDB]:
    db = get_database()
    return db.query(TaskDB).filter(
        TaskDB.id in map(
            lambda challenge_task: challenge_task.task_id,
            db.query(ChallengeTaskDB).filter(ChallengeTaskDB.challenge_id == challenge_id).all()
        )
    ).all()
    