#include<iostream>
﻿#include<string>
#include<stdlib.h>
#include<unistd.h>
#include<cstring>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<vector>
#include"user.h"
#include"message.h"
using namespace std;

#define MESSAGE_BUFFER 4096
#define PORT 7777

char buffer[MESSAGE_BUFFER];
char message[MESSAGE_BUFFER];
int socket_file_descriptor, message_size;
socklen_t length;
const char* end_string = "end";
struct sockaddr_in serveraddress, client;

int main()
{
    vector<User> users_vector; // создаем вектор пользователей класса User
    vector<Message> message_vector; // создаем вектор сообщений класса Message
    int num = 0; // переменная для меню

    // Создадим UDP сокет 
    socket_file_descriptor = socket(AF_INET, SOCK_DGRAM, 0);
    serveraddress.sin_addr.s_addr = htonl(INADDR_ANY);
    // Зададим порт для соединения
    serveraddress.sin_port = htons(PORT);
    // Используем IPv4
    serveraddress.sin_family = AF_INET;
    // Привяжем сокет 
    bind(socket_file_descriptor, (struct sockaddr*)&serveraddress, sizeof(serveraddress));

    //начало программы, читаем ответ клиента
    while (1)
    {
        cout << "start" << endl;
        //читаем из сети:
        length = sizeof(client);
        recvfrom(socket_file_descriptor, buffer, sizeof(buffer), 0, (struct sockaddr*)&client, &length);
        // в зависимости от ответа выставляем переменную num для попадания в соответствующее меню
        if (strncmp("1", buffer, 1) == 0)
        {
            num = 1;
        }
        if (strncmp("2", buffer, 1) == 0)
        {
            num = 2;
        }
        if (strncmp("end", buffer, 3) == 0)
        {
            close(socket_file_descriptor);
            exit(0);
        }
        if (num == 0)
        {
            // если ответ не 1 и не 2, то num остается равным 0, программа начинается с начала
            strcpy(message, "Вы не угадали. Меню:\n1 - Регистрация\n2 - Авторизация и работа в чате\nend - выход из программы");
            sendto(socket_file_descriptor, message, MESSAGE_BUFFER, 0, (struct sockaddr*)&client, sizeof(client));
        }
        // регистрация пользователя
        if (num == 1)
        {
            // запрашиваем логин
            strcpy(message, "регистрация введите ваш логин");
            sendto(socket_file_descriptor, message, MESSAGE_BUFFER, 0, (struct sockaddr*)&client, sizeof(client));

            //получаем логин
            length = sizeof(client);
            recvfrom(socket_file_descriptor, buffer, sizeof(buffer), 0, (struct sockaddr*)&client, &length);
            // записываем его в переменную
            string user_login(buffer);

            // запрашиваем имя
            strcpy(message, "регистрация введите ваше имя");
            sendto(socket_file_descriptor, message, MESSAGE_BUFFER, 0, (struct sockaddr*)&client, sizeof(client));

            //получаем имя
            length = sizeof(client);
            recvfrom(socket_file_descriptor, buffer, sizeof(buffer), 0, (struct sockaddr*)&client, &length);
            // записываем имя в переменную
            string user_name(buffer);

            // запрашиваем пароль
            strcpy(message, "регистрация введите ваш пароль");
            sendto(socket_file_descriptor, message, MESSAGE_BUFFER, 0, (struct sockaddr*)&client, sizeof(client));

            //получаем пароль
            length = sizeof(client);
            recvfrom(socket_file_descriptor, buffer, sizeof(buffer), 0, (struct sockaddr*)&client, &length);
            // записываем пароль в переменную
            string user_password(buffer);

            bool name_free = true;
            for (int i = 0; i < users_vector.size(); ++i) // просматриваем все зарегистрированные логины
            {
                if (user_login == users_vector[i].getUserLogin()) //если находим введенный логин в векторе пользователей 
                {
                    // сообщаем, что пользователь уже существует
                    strcpy(message, "пользователь с таким логином зарегистрироваан ранее, придумайте другой логин.\nНажмите 1 для регистрации\n2 для работы в чате\nend - завершение работы");
                    sendto(socket_file_descriptor, message, MESSAGE_BUFFER, 0, (struct sockaddr*)&client, sizeof(client));
                    // выставляем флаг свободного имени в false
                    name_free = false;
                    num = 0;
                }
            }
            //если логин свободен:
            if (name_free)
            {
                users_vector.push_back(User(user_login, user_name, user_password)); // добавляем новый объект User в вектор
                // сообщаем, что пользователь зарегистрирован
                strcpy(message, "Пользователь зарегистрирован, меню:\n1. Регистрация\n2. Работа в чате\nend - завершение работы");
                sendto(socket_file_descriptor, message, MESSAGE_BUFFER, 0, (struct sockaddr*)&client, sizeof(client));
                cout << "зарегистрирован пользователь " << user_login << endl;
            }
            num = 0;
        }
        // работа в чате, начиная с авторизации
        if (num == 2)
        {
            cout << "авторизация" << endl;
            // запрашиваем логин
            strcpy(message, "авторизация введите ваш логин");
            sendto(socket_file_descriptor, message, MESSAGE_BUFFER, 0, (struct sockaddr*)&client, sizeof(client));

            //получаем логин
            length = sizeof(client);
            recvfrom(socket_file_descriptor, buffer, sizeof(buffer), 0, (struct sockaddr*)&client, &length);
            // записываем его в переменную
            string user_login(buffer);

            // запрашиваем пароль
            strcpy(message, "авторизация введите ваш пароль");
            sendto(socket_file_descriptor, message, MESSAGE_BUFFER, 0, (struct sockaddr*)&client, sizeof(client));

            //получаем пароль
            length = sizeof(client);
            recvfrom(socket_file_descriptor, buffer, sizeof(buffer), 0, (struct sockaddr*)&client, &length);
            // записываем пароль в переменную
            string user_password(buffer);

            bool authorization = false;
            for (int i = 0; i < users_vector.size(); ++i) // перебираем введенные имя и пароль в векторах
            {
                if (user_login == users_vector[i].getUserLogin() && user_password == users_vector[i].getUserPassword()) // если нашли соответствие авторизация прошла
                {
                    // собираем фразу для ответа
                    string welcome = "Добро пожаловать в чат, " + users_vector[i].getUserName() + "\n1. отправка сообщения всем\n2. отправка личного сообщения\n3. получение сообщений.\nЛюбая другая кнопка - выход в предыдущее меню";
                    const char* welcome_answer = welcome.c_str();
                    //отправляем
                    sendto(socket_file_descriptor, welcome_answer, MESSAGE_BUFFER, 0, (struct sockaddr*)&client, sizeof(client));
                    authorization = true; //если имя и пароль верные, переменная authorization = true
                    cout << "авторизация прошла " << user_login << endl;
                    // cout << "1 " << buffer << endl;
                    num = 0;
                }
            }
            if (!authorization)
            {
                // если авторизация не удалась:
                strcpy(message, "Ошибка логина пользователя или пароля.\nНажмите 1 для регистрации, 2 для работы в чате, end - выход из программы");
                sendto(socket_file_descriptor, message, MESSAGE_BUFFER, 0, (struct sockaddr*)&client, sizeof(client));
                num = 0;
            }

            while (authorization)
            {
                int num_2 = 0;
                cout << "старт чат" << endl;
                // читаем ответ клиента
                length = sizeof(client);
                recvfrom(socket_file_descriptor, buffer, sizeof(buffer), 0, (struct sockaddr*)&client, &length);

                //в зависимости от ответа клиента, выставляем переменную num_2 для попадания в соответствующее меню
                if (strncmp("1", buffer, 1) == 0)
                {
                    num_2 = 1; // для рассылки всем
                }

                if (strncmp("2", buffer, 1) == 0)
                {
                    num_2 = 2; //для личного сообщения
                }

                if (strncmp("3", buffer, 1) == 0)
                {
                    num_2 = 3; // для получения сообщений
                }

                // общая рассылка:
                if (num_2 == 1)
                {
                    cout << "общая рассылка" << endl;
                    //просим клиента прислать текст для рассылки всем
                    strcpy(message, "напишите текст для общей рассылки");
                    sendto(socket_file_descriptor, message, MESSAGE_BUFFER, 0, (struct sockaddr*)&client, sizeof(client));

                    // читаем ответ клиента
                    length = sizeof(client);
                    recvfrom(socket_file_descriptor, buffer, sizeof(buffer), 0, (struct sockaddr*)&client, &length);
                    string current_message(buffer);

                    current_message = "сообщение всем от " + user_login + ": " + current_message;

                    for (int i = 0; i < users_vector.size(); ++i) // перебираем вектор User
                    {
                        if (user_login != users_vector[i].getUserLogin()) // для рассылки всем, кроме случая равенства имени текущего пользователя и пользователя из вектора User (чтобы не отправлять сообщение самому себе)
                        {
                            message_vector.push_back(Message(user_login, users_vector[i].getUserLogin(), current_message));// отправляем сообщение
                            //cout << "3 " << user_login << current_message << endl;
                        }
                    }
                    strcpy(message, "сообщение отправлено.\nВы в чате. 1. рассылка всем, 2. отправка личного сообщения. 3. получение\nдругая кнопка - выход в предыдущее меню");
                    sendto(socket_file_descriptor, message, MESSAGE_BUFFER, 0, (struct sockaddr*)&client, sizeof(client));
                }

                // личное сообщение:
                if (num_2 == 2)
                {
                    cout << "отправляем личное сообщение" << endl;
                    // отправка сообщения конкретному пользователю
                    bool sent_nosuccessfully = true; // значение переменной будет истина, пока пользователь не введет правильно имя получателя
                    string all_user;
                    for (int i = 0; i < users_vector.size(); ++i) // Отображаем на экран список зарегистрированных пользователей
                    {
                        all_user = users_vector[i].getUserLogin() + ", " + all_user;
                    }

                    all_user = "Сейчас в чате зарегистрированы: " + all_user + "кому отправляем сообщение?";

                    //отправляем клиенту список зарегистрированных пользователей
                    const char* message_all_user = all_user.c_str();
                    sendto(socket_file_descriptor, message_all_user, MESSAGE_BUFFER, 0, (struct sockaddr*)&client, sizeof(client));

                    // получаем ответ
                    length = sizeof(client);
                    recvfrom(socket_file_descriptor, buffer, sizeof(buffer), 0, (struct sockaddr*)&client, &length);

                    while (sent_nosuccessfully) // бесконечный цикл, пока не введем точное имя получателя
                    {
                        string current_reciption_name(buffer);

                        for (int i = 0; i < users_vector.size(); ++i) // перебираем введенное имя среди зарегистрированных пользователей
                        {
                            if (current_reciption_name == users_vector[i].getUserLogin()) // если находим, просим ввести текст сообщения
                            {
                                //просим ввести текст сообщения
                                strcpy(message, "введите сообщение");
                                sendto(socket_file_descriptor, message, MESSAGE_BUFFER, 0, (struct sockaddr*)&client, sizeof(client));

                                // получаем ответ
                                length = sizeof(client);
                                recvfrom(socket_file_descriptor, buffer, sizeof(buffer), 0, (struct sockaddr*)&client, &length);

                                string current_message(buffer); // преобразовали ответ в стринг
                                current_message = "Вам сообщение от " + user_login + ": " + current_message; //дополнили текст
                                message_vector.push_back(Message(user_login, current_reciption_name, current_message));//сохраняем сообщение в векторе

                                strcpy(message, "\nСообщение отправлено\n\nВы в чате. Меню:\n1. отправка сообщения всем\n2. отправка личного сообщения\n3. получение сообщений\nлюбая другая кнопка - выход в предыдущее меню");
                                sendto(socket_file_descriptor, message, MESSAGE_BUFFER, 0, (struct sockaddr*)&client, sizeof(client));

                                cout << "сообщение отправлено" << endl << endl;

                                sent_nosuccessfully = false; // устанавливаем флаг в положение "сообщение отправлено"
                            }
                        }
                        if (sent_nosuccessfully) // если имя получателя в списке не нашли, сообщаем об этом
                        {
                            string answer_no_user = "пользователь с именем " + current_reciption_name + " в чате не зарегистрирован\nКому отправляем сообщение?";
                            const char* answer_no_user_to_net = answer_no_user.c_str();
                            sendto(socket_file_descriptor, answer_no_user_to_net, MESSAGE_BUFFER, 0, (struct sockaddr*)&client, sizeof(client));

                            // ждем ответ
                            length = sizeof(client);
                            recvfrom(socket_file_descriptor, buffer, sizeof(buffer), 0, (struct sockaddr*)&client, &length);
                        }
                    }
                }

                // получение сообщений
                if (num_2 == 3)
                {
                    cout << "получаем сообщение" << endl;
                    int j = message_vector.size();
                    for (int i = 0; i < j; ++i)
                    {
                        if (user_login == message_vector[i].getRecipient()) // если находим свое имя в векторе Message в поле получатель, 
                        {
                            string temp = message_vector[i].getMessage() + "\nдля ответа лично нажмите '9', для ответа всем нажмите '5' для получения остальных сообщений - любую кнопку";
                            const char* message_temp = temp.c_str();
                            sendto(socket_file_descriptor, message_temp, MESSAGE_BUFFER, 0, (struct sockaddr*)&client, sizeof(client));

                            // получаем ответ
                            length = sizeof(client);
                            recvfrom(socket_file_descriptor, buffer, sizeof(buffer), 0, (struct sockaddr*)&client, &length);

                            if (strncmp("9", buffer, 1) == 0) // "9" - отпраляем личный ответ
                            {
                                // запрашиваем текст
                                strcpy(message, "введите текст сообщения:");
                                sendto(socket_file_descriptor, message, MESSAGE_BUFFER, 0, (struct sockaddr*)&client, sizeof(client));

                                //получаем текст
                                length = sizeof(client);
                                recvfrom(socket_file_descriptor, buffer, sizeof(buffer), 0, (struct sockaddr*)&client, &length);

                                string temp1(buffer); // занесли во временную переменную содержимое ответа
                                string answer_personal = "Вам сообщение от " + user_login + ": " + temp1; // "склеим ответ для помещения в вектор
                                message_vector.push_back(Message(user_login, message_vector[i].getSender(), answer_personal)); // отправляем сообщение
                            }

                            if (strncmp("5", buffer, 1) == 0) // если "5" - то отвечаем всем
                            {
                                // запрашиваем текст
                                strcpy(message, "введите текст сообщения:");
                                sendto(socket_file_descriptor, message, MESSAGE_BUFFER, 0, (struct sockaddr*)&client, sizeof(client));

                                //получаем текст
                                length = sizeof(client);
                                recvfrom(socket_file_descriptor, buffer, sizeof(buffer), 0, (struct sockaddr*)&client, &length);

                                string temp2(buffer); // занесли во временную переменную содержимое ответа
                                string answer_all = "сообщение всем от " + user_login + ": " + temp2; // "склеим" ответ для помещения в вектор

                                for (int k = 0; k < users_vector.size(); ++k) //отправляем всем сообщение
                                {
                                    if (user_login != users_vector[k].getUserLogin())
                                    {
                                        message_vector.push_back(Message(user_login, users_vector[k].getUserLogin(), answer_all));// сохраняем сообщение в векторе
                                    }
                                }
                            }
                            message_vector.erase(message_vector.begin() + i); // удаляем из вектора Message объект с прочитанным сообщением
                            --j; // длина векторов стала меньше на 1, соответственно уменьшаем на 1 количество проходов
                            --i; // удалено текущее значения в векторе Message, значит с этим же индексом хранится уже новое сообщение, оно проверяется на следующем проходе
                        }
                    }
                    strcpy(message, "Входящих сообщений нет, но вы в чате, меню:\n1. отправка сообщения всем\n2. отправка личного сообщения\n3. получение сообщений.\nлюбая другая кнопка - выход в предыдущее меню");
                    sendto(socket_file_descriptor, message, MESSAGE_BUFFER, 0, (struct sockaddr*)&client, sizeof(client));
                }
                // если ответ не 1, 2 или 3, то переходим в предыдущее меню, для этого:
                if (num_2 == 0)
                {
                    authorization = false;
                    cout << "авторизация false" << endl;
                    num = 0;
                    strcpy(message, "Вы снова в начале, меню:\n1 - Регистрация\n2 - Авторизация и работа в чате\nend - выход из программы");
                    sendto(socket_file_descriptor, message, MESSAGE_BUFFER, 0, (struct sockaddr*)&client, sizeof(client));
                }
            }
        }
        cout << "finish" << endl;
    }
    return 0;
}