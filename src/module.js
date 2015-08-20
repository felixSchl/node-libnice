'use strict';

var libnice = require('../build/Debug/native_libnice')
  , EventEmitter = require('events').EventEmitter;

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

function inherits(target, source) {
  for (var k in source.prototype) {
    target.prototype[k] = source.prototype[k];
  }
}

inherits(libnice.Agent, EventEmitter);
inherits(libnice.Stream, EventEmitter);

var agentL = new libnice.Agent()
  , streamL = agentL.addStream();

agentL.iceTcp = false;
agentL.controllingMode = false;
streamL.name = 'application';
streamL.on('gathering-done', function() {
  console.log('agentL - gathering done');
  // var sdpL = agentL.generateLocalSdp();
  // console.log('agentL - sdp:', sdpL);
});
streamL.gatherCandidates();

var agentR = new libnice.Agent()
  , streamR = agentR.addStream();

agentR.iceTcp = false;
agentR.controllingMode = false;
streamR.name = 'application';
streamR.on('gathering-done', function() {
  console.log('agentR - gathering done');
  // var sdpR = agentR.generateLocalSdp();
  // console.log('agentR - sdp:', sdpR);
});
streamR.gatherCandidates();
