'use strict';

var libnice = require('../build/Release/native_libnice')
  , EventEmitter = require('events').EventEmitter;

function inherits(target, source) {
  for (var k in source.prototype) {
    target.prototype[k] = source.prototype[k];
  }
}

inherits(libnice.Agent, EventEmitter);
inherits(libnice.Stream, EventEmitter);

var agent = new libnice.Agent()
  , stream = agent.addStream();

stream.name = 'application';
stream.on('gathering-done', function() {
  console.log('candidates gathered for stream', stream.id);
  console.log(agent.generateLocalSdp());
});

stream.gatherCandidates();

