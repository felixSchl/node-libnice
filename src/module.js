'use strict';

var libnice = require('../build/Release/native_libnice')
  , EventEmitter = require('events').EventEmitter;

/**
 * Easy access to `NiceComponentState`
 */

var NiceComponentState = {
  DISCONNECTED: 'DISCONNECTED'
, GATHERING: 'GATHERING'
, CONNECTING: 'CONNECTING'
, CONNECTED: 'CONNECTED'
, READY: 'READY'
, FAILED: 'FAILED'
, LAST: 'LAST'
};

NiceComponentState[0] = NiceComponentState.DISCONNECTED;
NiceComponentState[1] = NiceComponentState.GATHERING;
NiceComponentState[2] = NiceComponentState.CONNECTING;
NiceComponentState[3] = NiceComponentState.CONNECTED;
NiceComponentState[4] = NiceComponentState.READY;
NiceComponentState[5] = NiceComponentState.FAILED;
NiceComponentState[6] = NiceComponentState.LAST;

/**
 * Inherit from `EventEmitter`
 */

function inherits(target, source) {
  for (var k in source.prototype) {
    target.prototype[k] = source.prototype[k];
  }
}

inherits(libnice.Agent, EventEmitter);
inherits(libnice.Stream, EventEmitter);

/**
 * Export subset
 */

module.exports = {
  Agent: libnice.Agent
, NiceComponentState: NiceComponentState
};
