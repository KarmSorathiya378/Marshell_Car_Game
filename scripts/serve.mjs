import http from 'node:http';
import { createReadStream, existsSync, statSync } from 'node:fs';
import path from 'node:path';
import { fileURLToPath } from 'node:url';
const root = fileURLToPath(new URL('../VelocityX_Web/',import.meta.url));
const port = Number(process.env.PORT || 8000);
const mime={'.html':'text/html; charset=utf-8','.js':'application/javascript','.css':'text/css','.json':'application/json','.wasm':'application/wasm','.png':'image/png'};
http.createServer((request,response)=>{
  let file;
  try {file=path.resolve(root,'.'+decodeURIComponent(new URL(request.url,'http://localhost').pathname));}
  catch {response.writeHead(400);response.end();return;}
  if(file!==root.slice(0,-1)&&!file.startsWith(root)){response.writeHead(403);response.end();return;}
  if(existsSync(file)&&statSync(file).isDirectory())file=path.join(file,'index.html');
  if(!existsSync(file)){response.writeHead(404);response.end('Not found');return;}
  response.setHeader('Content-Type',mime[path.extname(file)]||'application/octet-stream');
  response.setHeader('Cache-Control','no-store');createReadStream(file).pipe(response);
}).listen(port,'127.0.0.1',()=>console.log(`Velocity X: http://127.0.0.1:${port}`));
