import telebot
from telebot import types
import pika
import yaml
import json

with open("config.yaml", "r") as file:
    data = yaml.safe_load(file)
token = data['token']
bot = telebot.TeleBot(token)
help_text = "Это бот для сдачи решений по программированию в контест систему (ДТС)"
user_states = {}


@bot.message_handler(commands=['start'])
def start_message(message):
    keyboard = types.ReplyKeyboardMarkup(resize_keyboard=True)
    button1 = types.KeyboardButton("Сдать решение")
    button2 = types.KeyboardButton("help")
    keyboard.add(button1, button2)
    bot.send_message(message.chat.id, "Привет это бот для проекта по питону", reply_markup=keyboard)


@bot.message_handler(func=lambda message: message.text == 'Сдать решение')
def handle_button1(message):
    user_states[message.chat.id] = 'Сдать решение'
    bot.register_next_step_handler(message, compilation_choice)


def compilation_choice(message):
    solution_text = message.text
    keyboard = types.ReplyKeyboardMarkup(resize_keyboard=True)
    button1 = types.KeyboardButton("gcc")
    button2 = types.KeyboardButton("python")
    keyboard.add(button1, button2)
    bot.send_message(message.chat.id, "Выберите компилятор для решения.", reply_markup=keyboard)
    bot.register_next_step_handler(message, lambda message: send_solution(message, solution_text))


def send_solution(message, solution_text):
    if message.text == 'gcc':
        c_plus_plus_solution(message, solution_text)
    elif message.text == 'python':
        pass


def c_plus_plus_solution(message, solution_text):
    connection = pika.BlockingConnection(pika.ConnectionParameters('localhost'))
    channel = connection.channel()

    channel.queue_declare(queue='test')
    json_message = {
        "submissionId": 0,
        "buildId": 0,
        "userData": solution_text,
        "taskId": 0,
        "memoryLimit": 10241024,
        "cpuTimeLimitMilliSeconds": 2000
    }

    print(json.dumps(json_message))

    channel.basic_publish(
        exchange='',
        routing_key='test',
        body=json.dumps(json_message)
    )
    bot.send_message(message.chat.id, 'Ваше решение отправлено на сервер!')


@bot.message_handler(func=lambda message: message.text == 'help')
def handle_button2(message):
    user_states[message.chat.id] = 'help'
    bot.send_message(message.chat.id, help_text)


bot.infinity_polling()