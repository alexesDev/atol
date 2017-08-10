var addon = require('bindings')('addon');
var kkm = new addon.Atol('./atol/libfptr.so', 'ttyACM0');

kkm.printText("hello from node.js\n\n\n");
kkm.printFooter();
