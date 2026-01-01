# ESP32 Wi-Fi Scan Web

Навчальний приклад для ESP32, який демонструє
сканування Wi-Fi точок доступу під HTTP-запит
з виводом результатів у браузер.

## Можливості
- Робота в режимі Wi-Fi STA (DHCP)
- Сканування Wi-Fi лише при відкритті сторінки
- HTML-вивід у браузер
- Відладка через Serial Monitor

## Апаратна платформа
Перевірено на:
- ESP32 Dev Module (ESP32-D0WD-V3)

## Середовище розробки
- Arduino IDE (ESP32 Arduino Core)
- PlatformIO (Arduino framework) — очікується коректна робота

## Запуск
1. Відкрити скетч в Arduino IDE
2. Вказати Wi-Fi SSID та пароль
3. Завантажити в ESP32
4. Відкрити `http://<IP_ESP32>/`

## Документація
- Розширені навчальні нотатки: `docs/README.educational.ua.md`

## Ліцензія
MIT
