var Benchmark = require('benchmark');
var R3 = require('./index.js');

var router = new R3();
router.insert('/exobot/help');
router.insert('/exobot/help/{search}');
router.insert('/exobot/permissions/give/{a}/to/{b}');
router.insert('/exobot/permissions/give/{a}/{b}');
router.insert('/exobot/permissions/view/{a}');
router.insert('/exobot/permissions/webhook/give/{a}/to/{b}');
router.insert('/exobot/permissions/webhook/give/{a}/{b}');
router.insert('/exobot/permissions/webhook/view/{a}');
router.insert('/exobot/uptime');
router.compile();

var regexList = [
  /\/exobot\/help/i,
  /\/exobot\/help\/(\S+)/i,
  /\/exobot\/permissions\/give\/(\S+)\/to\/(\S+)/i,
  /\/exobot\/permissions\/give\/(\S+)\/(\S+)/i,
  /\/exobot\/permissions\/view\/(\S+)/i,
  /\/exobot\/permissions\/webhook\/give\/(\S+)\/to\/(\S+)/i,
  /\/exobot\/permissions\/webhook\/give\/(\S+)\/(\S+)/i,
  /\/exobot\/permissions\/webhook\/view\/(\S+)/i,
  /\/exobot\/uptime/i,
];

var commandList = [
  '/exobot/help',
  '/exobot/help/search',
  '/exobot/permissions/give/a/to/b',
  '/exobot/permissions/give/a/b',
  '/exobot/permissions/view/a',
  '/exobot/permissions/webhook/give/a/to/b',
  '/exobot/permissions/webhook/give/a/b',
  '/exobot/permissions/webhook/view/a',
  '/exobot/uptime',
  '/exobot/garbage',
  '/exobot/ham',
  '/exobot/sunny',
];

var suite = new Benchmark.Suite;

// add tests
suite.add('r3', function() {
  commandList.forEach(c => router.match(c))
})
suite.add('regexes', function() {
  commandList.forEach(c => regexList.find(r => r.test(c)))
})
.on('cycle', function(event) {
  console.log(String(event.target));
})
.on('complete', function() {
  console.log('Fastest is ' + this.filter('fastest').map('name'));
})
// run async
.run({ 'async': true });
