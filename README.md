# Nixie-Cloud-Weatherstation

Комплексное решение облачной метеостанции с выходом в облако.
Уже сейчас работает основной вывод данных в облако ThingSpeak, но еще много чего нужно сделать.

Апдейт от 05.05.21
Сделано:
Написана программа для смены логина-пароля без перепрошивки, в мк добавлен соответствующий алгоритм
Выбор между записью-чтением ЕЕпром
Выбор между WPS и стандартным подключением
Оптимизация использования памяти


Нужно сделать:
Гипотетически возможна ситуация, когда источник интернета(роутер) и сопроцессор включатся одновременно, тогда есп не запомнит параметры сети - не исправлено
В логине и пароле не должно быть запятой, потому что парсер отличает логин от пароля именно по запятой - лучше сделать перенос каретки


