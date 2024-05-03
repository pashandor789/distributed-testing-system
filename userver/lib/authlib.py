import time
import environs

from jose import jwt

env = environs.Env()
env.read_env()
ACCESS_TOKEN_EXPIRE_MINUTES = env.int("ACCESS_TOKEN_EXPIRE_MINUTES")
ALGORITHM = env("ALGORITHM")
PRIVATE_KEY = env("PRIVATE_KEY")


def parse_token(token: str):
    return jwt.decode(token, PRIVATE_KEY, algorithms=[ALGORITHM])

def verify_token(token: str):
    return parse_token(token)["end_timestamp"] > time.time()

def get_user_id_from_token(token: str):
    if not verify_token(token):
        raise RuntimeError("token is invalid")
    return parse_token(token)["user_id"]

def create_token(user_id: str):
    return jwt.encode(
        {
            "user_id": user_id,
            "end_timestamp": int(time.time()) + ACCESS_TOKEN_EXPIRE_MINUTES
        },
        PRIVATE_KEY,
        algorithm=ALGORITHM
    )
