# Обертка над драйвером под node.js для FPrint-22ПТК

Очень мало инфы вообще про разработку ПО для Atol, в какой-то момент я
даже не знал, что есть офф библиотека под Linux для c++ (хотя и с
закрытыми исходниками). В данном репозитории просто обётка под node.js,
сделанная на коленке.

Двайвер поддерживает все кассы от Atol, но тут все жестко на
FPrint-22ПТК и общение через serial (задано в конструкторе Atol).

## Пример

```js
const addon = require('bindings')('addon');
const kkm = new addon.Atol('./atol/libfptr.so', 'ttyACM0', addon.ModelFPrint22PTK, addon.ProtocolAtol30);

kkm.printText("hello from node.js\n\n\n");
kkm.printFooter();
```

## API

### `new Atol(libPath, tty, model, protocol)`

Создаёт основной объект для работы с kkm.

#### Arguments

 - `libPath` (string): путь к libfptr.so
 - `tty` (string): название устройства (обычно ttyACM0)
 - `model` (int): `ModelFPrint22K` или `ModelFPrint22PTK` (константы из пакета)
 - `protocol` (int): `ProtocolAtol20` или `ProtocolAtol30` (константы из пакета)

#### Methods

 - ` printText` (string)
 - ` printFooter` ()
 - ` openCheck` (integer): открытие чека, принимает константу вида ChequeSell
 - ` zReprot` ()

### -3813 Неверная величина скидки/надбавки

Скидки на чек запрещены в старом виде. Теперь только в рамках копеек общей суммы,
например от 100.83 можно отбросить до 83 копеек. Таково новое законодательство.

Т.е. метод discount() для чека в новых кассах в принципе не работает...
они решили учитывать каждую позицию и чтобы не париться "что делать со
скидкой на чек" просто выкинули такую фишку. Пусть разруливает народ.

## Официальный драйвер

Файлы взяты из [официального драйвера](http://fs.atol.ru/SitePages/%D0%A6%D0%B5%D0%BD%D1%82%D1%80%20%D0%B7%D0%B0%D0%B3%D1%80%D1%83%D0%B7%D0%BA%D0%B8.aspx?raz1=%D0%9F%D1%80%D0%BE%D0%B3%D1%80%D0%B0%D0%BC%D0%BC%D0%BD%D0%BE%D0%B5+%D0%BE%D0%B1%D0%B5%D1%81%D0%BF%D0%B5%D1%87%D0%B5%D0%BD%D0%B8%D0%B5&raz2=%D0%94%D0%A2%D0%9E)

## TODO

- вынести функции вроде put_Summ через аддон, а саму логику с проверками аргументов перенести в JS
