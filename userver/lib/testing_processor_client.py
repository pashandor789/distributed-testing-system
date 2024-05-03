import pika
import json
import environs

env = environs.Env()
env.read_env()
QUEUE_HOST = env("QUEUE_HOST")
QUEUE_NAME = env("QUEUE_NAME")

def send_solution(
        solution: str,
        submission_id: int,
        build_id: int,
        task_id: int,
        memory_limit: int,
        time_limit: int
    ):
    connection = pika.BlockingConnection(pika.ConnectionParameters(QUEUE_HOST))
    channel = connection.channel()
    channel.queue_declare(queue=QUEUE_NAME)
    json_message = {
        "submissionId": submission_id,
        "buildId": build_id,
        "userData": solution,
        "taskId": task_id,
        "memoryLimit": memory_limit * 1024 * 1024,
        "cpuTimeLimitMilliSeconds": time_limit * 1000
    }


    channel.basic_publish(
        exchange="",
        routing_key=QUEUE_NAME,
        body=json.dumps(json_message)
    )
