from sqlalchemy import Column, Integer, String, ForeignKey

from .session import get_database, get_engine, Base

class UserDB(Base):
    __tablename__ = "users"

    id = Column(Integer, primary_key=True, index=True)
    name = Column(String, unique=True, index=True)
    password = Column(String)

Base.metadata.create_all(bind=get_engine())

def add_user(username: str, password: str):
    db = get_database()
    if db.query(UserDB).filter(UserDB.name == username).count() != 0:
        raise RuntimeError("user with this username already exists")
    user = UserDB(name=username, password=str(hash(username + password)))
    db.add(user)
    db.commit()
    db.refresh(user)

def get_user(username: str):
    db = get_database()
    print(*map(lambda x: x.__dict__, db.query(UserDB).all()))

    user = db.query(UserDB).filter(UserDB.name == username).first()
    if not user:
        raise RuntimeError("user with this username doesn't exist")
    return user
