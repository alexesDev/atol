const addon = require('bindings')('addon');

test('ProtocolAtol20 constant', () => {
  expect(addon.ProtocolAtol20).toBe(1);
});

test('ProtocolAtol30 constant', () => {
  expect(addon.ProtocolAtol30).toBe(2);
});

test('ModelFPrint22NEW constant', () => {
  expect(addon.ModelFPrint22PTK).toBe(63);
});

test('ModelFPrint22K constant', () => {
  expect(addon.ModelFPrint22K).toBe(52);
});
