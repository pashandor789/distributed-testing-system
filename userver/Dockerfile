FROM python:3.9
WORKDIR /server
COPY requirements.txt .
RUN pip install -r requirements.txt
COPY . .
CMD ["uvicorn", "server:app", "--reload", "--host", "0.0.0.0", "--port", "8003"]
