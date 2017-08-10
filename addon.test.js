const addon = require('bindings')('addon');

test('ProtocolAtol20 constant', () => {
  expect(addon.ProtocolAtol20).toBe(1);
});

test('ProtocolAtol30 constant', () => {
  expect(addon.ProtocolAtol30).toBe(2);
});
