'use strict';

var libnice = require('../build/Release/native_libnice')
  , util = require('util')
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

console.log(util.inspect(stream));
console.log('stream name:', stream.name);

stream.on('gathering-done', function() {
  console.log('awesome');
});

console.log(stream.gatherCandidates());
console.log(stream.gatherCandidates());
console.log(stream.gatherCandidates());
