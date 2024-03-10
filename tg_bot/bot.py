import telebot
from telebot import types
import pika

token = '7026760925:AAHpUd9zOHVm_Vmo_OqJ1Kgc4AlRNvUQSG4'
bot = telebot.TeleBot(token)
help_text = "Это бот для сдачи решений по программированию в контест систему (ДТС)"


@bot.message_handler(commands=['start'])
def start_message(message):
    bot.send_message(message.chat.id, "Привет это бот для проекта по питону")
    keyboard = types.InlineKeyboardMarkup(row_width=2)
    button1 = types.InlineKeyboardButton("Сдать решение", callback_data='btn1')
    button2 = types.InlineKeyboardButton("help", callback_data='btn2')
    keyboard.add(button1, button2)


@bot.callback_query_handler(func=lambda call: True)
def callback_handler(call):
    if call.data == 'btn1':
        bot.send_message(call.message.chat.id, "Загрузите своей решение файлом, либо текстом.")
    elif call.data == 'btn2':
        bot.send_message(call.message.chat.id, help_text)

    @bot.message_handler(content_types=['text'])
    def handle_text(message):
        text_input = message.text
        bot.send_message(message.chat.id, f"You entered text: {text_input}")

    @bot.message_handler(content_types=['document'])
    def handle_file(message):
        file = bot.get_file(message.document.file_id)
        downloaded_file = bot.download_file(file.file_path)
        with open("file_received.txt", 'wb') as file:
            file.write(downloaded_file)

        bot.send_message(message.chat.id, "File successfully saved.")


bot.infinity_polling()