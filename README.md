# dead_drop_service_prototype


## Суть сервиса

Вы, как отправитель имеете возможность положить данные в почтовый ящик, от которого вам достанутся «ключи» (пароль) и уникальный «адрес» (ссылка). Просто передайте получателю ключ с адресом по разным каналам связи, например, по телефону назовите пароль, а ссылку отправьте в любом удобном мессенджере. Как только получатель заберет «посылку» (файлы или текст) из ячейки – она безвозвратно исчезнет. Прямого контакта между отправителем и получателем нет.

Это безопасно: перехватив пароль без ссылки – злоумышленник не узнает где его применить, получив адрес без пароля – злоумышленник не сможет открыть его. 


## Dead Drop Service Walkthrough


### What was built
- Core Logic: SecretManager (in-memory, thread-safe).
- Web Server: C++ Crow application running on port 8080.
- Frontend: Simple HTML templates for creating and viewing secrets.
- Build System: CMake with CPM dependency management (Crow, nlohmann/json, asio).

### Verification Results
Ran automated tests in verify.py:

1) Create Secret: POST /api/secret -> Success (Got ID).
2) First View: GET /secret/<id> -> Success (Content retrieved).
3) Second View: GET /secret/<id> -> Success (404 Not Found - Correctly deleted).

### How to run
1) Build:
```
mkdir build && cd build
cmake ..
cmake --build .
```

2) Run:
```
./dead_drop_service
```
3) Use: Open http://localhost:8080 in your browser.