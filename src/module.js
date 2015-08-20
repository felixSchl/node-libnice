'use strict';

var libnice = require('../build/Debug/native_libnice')
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
 * `NiceCompatibility`
 */

var NiceCompatibility = {
  RFC5245: 'RFC5245'
, DRAFT19: 'DRAFT19'
, GOOGLE: 'GOOGLE'
, MSN: 'MSN'
, WLM2009: 'WLM2009'
, OC2007: 'OC2007'
, OC2007R2: 'OC2007R2'
};

NiceCompatibility[0] = NiceCompatibility.RFC5245;
NiceCompatibility[1] = NiceCompatibility.DRAFT19;
NiceCompatibility[2] = NiceCompatibility.GOOGLE;
NiceCompatibility[3] = NiceCompatibility.MSN;
NiceCompatibility[4] = NiceCompatibility.WLM2009;
NiceCompatibility[5] = NiceCompatibility.OC2007;
NiceCompatibility[6] = NiceCompatibility.OC2007R2;

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
inherits(libnice.Component, EventEmitter);

/**
 * Export subset
 */

module.exports = {
  Agent: libnice.Agent
, NiceComponentState: NiceComponentState
};
