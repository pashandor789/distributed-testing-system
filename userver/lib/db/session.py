from sqlalchemy import create_engine
from sqlalchemy.orm import sessionmaker, Session
from sqlalchemy.ext.declarative import declarative_base


from environs import Env

env = Env()
env.read_env()

DATABASE_URL = env("DATABASE_URL")

_engine = create_engine(
    DATABASE_URL, connect_args={"check_same_thread": False}
)

_sessionLocal = sessionmaker(autocommit=False, autoflush=False, bind=_engine)

Base = declarative_base()

def get_database() -> Session:
    database = _sessionLocal()
    try:
        return database
    finally:
        database.close()

def get_engine():
    return _engine