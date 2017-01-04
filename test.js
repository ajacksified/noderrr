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

const commandList = [
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

commandList.forEach(c => {
  //console.log('matching', c);
  console.log(router.match(c));
  //console.log('next...');
})
