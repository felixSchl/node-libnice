'use strict';

var libnice = require('../build/Release/native_libnice')
  , EventEmitter = require('events').EventEmitter;

var NiceComponentState = {
  DISCONNECTED: 'DISCONNECTED'
, GATHERING: 'GATHERING'
, CONNECTING: 'CONNECTING'
, CONNECTED: 'CONNECTED'
, READY: 'READY'
, FAILED: 'FAILED'
, LAST: 'LAST'
}

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

var agent = new libnice.Agent()
  , stream = agent.addStream();

agent.iceTcp = false;
stream.name = 'application';

stream.on('gathering-done', function() {
  console.log('candidates gathered for stream', stream.id);
  var localSdp = agent.generateLocalSdp();
  console.log('local sdp', localSdp);
  console.log('parsing remote sdp', localSdp);
  var numCandidatesAdded = agent.parseRemoteSdp(localSdp);
  console.log('Remote sdp parsed');
  console.log('number of candidates added:', numCandidatesAdded);
});

// stream.on('state-changed', function(state, componentId) {
//   console.log(
//     'state changed for component', componentId
//   , ':', NiceComponentState[state]);
// });

stream.gatherCandidates();
