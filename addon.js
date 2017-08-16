const addon = require('bindings')('addon');
const kkm = new addon.Atol('./atol/libfptr.so', 'ttyACM0', addon.ModelFPrint22PTK, addon.ProtocolAtol30);

kkm.openCheck(addon.ChequeSell);
kkm.registrationFZ54("Яблоки", 1, 1, addon.DiscountSumm, 0, 1);
kkm.closeCheck(0);
