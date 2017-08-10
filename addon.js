const addon = require('bindings')('addon');
const kkm = new addon.Atol('./atol/libfptr.so', 'ttyACM0', addon.ModelFPrint22PTK, addon.ProtocolAtol30);

kkm.printText("hello from node.js\n\n\n");
kkm.printFooter();
