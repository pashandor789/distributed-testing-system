from typing import Any

def delete_techincal_info(db_ans: Any, fields: list[str]):
    result = db_ans.__dict__
    for field in fields:
        del result[field]
    return result