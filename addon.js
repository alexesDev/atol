const addon = require('bindings')('addon');
const kkm = new addon.Atol('./atol/libfptr.so', 'ttyACM1', addon.ModelFPrint22PTK, addon.ProtocolAtol30);

try {
  kkm.cancelCheck();
} catch (e) {
  console.log(e);
}

try {
  kkm.openCheck(addon.ChequeSell);
  kkm.registrationFZ54("Яблоки", 2, 2.2, addon.DiscountPercent, 0, 1);
  kkm.discount(0.4, 0, 0);
  kkm.closeCheck(0);
} catch (e) {
  console.log(e);
  kkm.cancelCheck();
}
