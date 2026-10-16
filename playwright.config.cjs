const { defineConfig, devices } = require('@playwright/test');
module.exports = defineConfig({
  testDir:'./tests/browser',timeout:60000,expect:{timeout:15000},fullyParallel:false,workers:1,
  reporter:'list',use:{baseURL:'http://127.0.0.1:8000',viewport:{width:1440,height:900},trace:'retain-on-failure'},
  projects:[
    {name:'desktop',use:{...devices['Desktop Chrome'],launchOptions:{
      executablePath:process.env.VX_BROWSER_PATH || undefined,
      args:['--enable-webgl','--use-angle=swiftshader','--enable-unsafe-swiftshader']
    }}},
    {name:'mobile',use:{...devices['Pixel 7'],defaultBrowserType:'chromium',launchOptions:{
      executablePath:process.env.VX_BROWSER_PATH || undefined,
      args:['--enable-webgl','--use-angle=swiftshader','--enable-unsafe-swiftshader']
    }}}
  ],
  webServer:{command:'node scripts/serve.mjs',url:'http://127.0.0.1:8000',reuseExistingServer:!process.env.CI,timeout:15000}
});
