const { test } = require('node:test');
const assert = require('node:assert/strict');
const { readFileSync } = require('node:fs');
const path = require('node:path');
const vm = require('node:vm');

const webRoot = path.resolve(__dirname, '../VelocityX_Web');
const html = readFileSync(path.join(webRoot, 'index.html'), 'utf8');
const scripts = [...html.matchAll(/<script\b([^>]*)>([\s\S]*?)<\/script>/gi)];

// Execute the shipped scripts with real Three.js geometry and a small DOM surface.
// Only the GPU renderer is replaced: these tests do not validate WebGL or pixels.
function loadGame({ missingEngine = false, brokenWebGL = false } = {}) {
  const elements = new Map([...html.matchAll(/\bid="([^"]+)"/g)].map(([, id]) => [id, {
    style: {}, disabled: id === 'drive-button', textContent: '', innerHTML: '', innerText: ''
  }]));
  const documentEvents = new Map();
  const windowEvents = new Map();
  const frames = [];
  const errors = [];
  const rendered = [];
  const sandbox = {
    innerWidth: 1280,
    innerHeight: 720,
    document: {
      getElementById: id => elements.get(id) ?? null,
      body: { appendChild() {} },
      addEventListener: (name, fn) => documentEvents.set(name, fn)
    },
    addEventListener: (name, fn) => windowEvents.set(name, fn),
    requestAnimationFrame: fn => frames.push(fn),
    console: { log() {}, warn() {}, error: (...args) => errors.push(args) }
  };
  sandbox.window = sandbox;
  sandbox.self = sandbox;
  // Browser globals such as window.top cannot be redeclared with top-level const.
  Object.defineProperty(sandbox, 'top', { value: sandbox, configurable: false });
  const context = vm.createContext(sandbox);
  vm.runInContext('var top;', context);
  for (const [, attributes, code] of scripts) {
    const src = attributes.match(/\bsrc="([^"]+)"/);
    if (src) {
      if (missingEngine) continue;
      vm.runInContext(readFileSync(path.join(webRoot, src[1]), 'utf8'), context);
      sandbox.THREE.WebGLRenderer = class {
        constructor() {
          if (brokenWebGL) throw new Error('WebGL unavailable');
          this.shadowMap = {};
          this.domElement = {};
        }
        setSize() {}
        render(scene, camera) { rendered.push({ scene, camera }); }
      };
    } else {
      vm.runInContext(code, context, { filename: 'index.html inline script' });
    }
  }
  return {
    elements, sandbox, frames, errors, rendered,
    key(key, type = 'keydown') { documentEvents.get(type)({ key, repeat: false }); },
    blur() { windowEvents.get('blur')(); },
    tick(count = 1) {
      for (let i = 0; i < count; i++) {
        assert.equal(frames.length, 1, 'The game must schedule exactly one animation loop');
        frames.shift()();
      }
    }
  };
}

test('all script assets ship locally and every script has valid syntax', () => {
  assert.ok(scripts.length > 0);
  for (const [, attributes, code] of scripts) {
    const src = attributes.match(/\bsrc="([^"]+)"/);
    if (src) {
      assert.ok(src[1].startsWith('./'), 'Engine must not depend on an external CDN');
      new vm.Script(readFileSync(path.join(webRoot, src[1]), 'utf8'));
    } else {
      new vm.Script(code);
    }
  }
});

test('startup succeeds with browser globals and enables Drive', () => {
  const game = loadGame();
  assert.deepEqual(game.errors, []);
  assert.equal(game.elements.get('drive-button').disabled, false);
  assert.match(game.elements.get('load-status').textContent, /Ready/);
  assert.equal(typeof game.sandbox.startGame, 'function');
  assert.equal(game.rendered.length, 1);
});

test('Drive starts one loop and W accelerates the car forward', () => {
  const game = loadGame();
  game.sandbox.startGame();
  game.sandbox.startGame();
  assert.equal(game.elements.get('main-menu').style.display, 'none');
  assert.equal(game.elements.get('hud').style.display, 'block');
  const car = game.rendered[0].scene.children.find(child => child.type === 'Group');
  assert.ok(car, 'Scene must contain the car');
  const initialZ = car.position.z;
  game.key('w');
  game.tick(60);
  game.key('w', 'keyup');
  assert.ok(parseInt(game.elements.get('speedometer').innerHTML, 10) > 0);
  assert.ok(car.position.z > initialZ, 'Accelerating must move toward the road');
  assert.deepEqual(game.errors, []);
});

test('Escape pauses movement and Resume continues it', () => {
  const game = loadGame();
  game.sandbox.startGame();
  game.key('w');
  game.tick(20);
  game.key('Escape');
  assert.equal(game.elements.get('pause-menu').style.display, 'block');
  const pausedSpeed = game.elements.get('speedometer').innerHTML;
  game.tick(20);
  assert.equal(game.elements.get('speedometer').innerHTML, pausedSpeed);
  game.sandbox.resumeGame();
  assert.equal(game.elements.get('pause-menu').style.display, 'none');
  game.tick(20);
  assert.ok(parseInt(game.elements.get('speedometer').innerHTML, 10) > parseInt(pausedSpeed, 10));
});

test('losing focus pauses the game and clears held acceleration', () => {
  const game = loadGame();
  game.sandbox.startGame();
  game.key('w');
  game.tick(20);
  game.blur();
  assert.equal(game.elements.get('pause-menu').style.display, 'block');
  const speed = parseInt(game.elements.get('speedometer').innerHTML, 10);
  game.sandbox.resumeGame();
  game.tick(20);
  assert.ok(parseInt(game.elements.get('speedometer').innerHTML, 10) < speed);
});

for (const [name, options] of [
  ['missing engine', { missingEngine: true }],
  ['unavailable WebGL', { brokenWebGL: true }]
]) {
  test(`${name} shows a startup error and disables Drive`, () => {
    const game = loadGame(options);
    assert.match(game.elements.get('load-status').textContent, /Unable to load/);
    assert.equal(game.elements.get('drive-button').disabled, true);
    assert.equal(game.errors.length, 1);
    assert.equal(game.frames.length, 0);
  });
}
