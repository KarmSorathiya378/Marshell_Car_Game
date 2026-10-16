// Browser port of VelocityX/Source/VelocityX/Private/Drive/DriveCar.cpp.
// The 120 Hz arcade dynamics, tuning, ordered gates and three-lap rules are
// preserved. Unreal collision sweeps are replaced by an oval barrier constraint.
#include <cmath>
#include <algorithm>

namespace vx {
constexpr float pi = 3.14159265358979323846f;
constexpr float dt = 1.f / 120.f;
struct Vec {
    float x, y;
    Vec operator+(Vec b) const { return {x+b.x, y+b.y}; }
    Vec operator-(Vec b) const { return {x-b.x, y-b.y}; }
    Vec operator*(float s) const { return {x*s, y*s}; }
};
float length(Vec v) { return std::sqrt(v.x*v.x+v.y*v.y); }
Vec normalized(Vec v) { float n=length(v); return n>0.00001f ? v*(1.f/n) : Vec{0,0}; }
float dot(Vec a,Vec b) { return a.x*b.x+a.y*b.y; }
Vec point(float a) { return {40000.f*std::cos(a),24000.f*std::sin(a)}; }
Vec tangent(float a) { return normalized({-40000.f*std::sin(a),24000.f*std::cos(a)}); }
float angle(Vec p) { float a=std::atan2(p.y/24000.f,p.x/40000.f); return a<0 ? a+2*pi : a; }
float approach(float from,float to,float change) { return from+std::clamp(to-from,-change,change); }
struct Game {
    Vec position{40000,0}, motion{0,0};
    float yaw=pi/2, speed=0, steering=0, boost=1;
    float raceTime=0,lapTime=0,bestLap=0,lastLap=0,countdown=3,noticeTime=0,accumulator=0;
    int lap=1,nextGate=1,carType=0,notice=0;
    bool started=false,finished=false,offroad=false,boosting=false;
};
void recover(Game& g) {
    float a=(g.nextGate-1)*2*pi/16;
    g.position=point(a);
    Vec t=tangent(a); g.yaw=std::atan2(t.y,t.x);
    g.speed=0;g.motion={0,0};g.steering=0;g.accumulator=0;
}
bool crosses(Vec previous,Vec current,float speed,int gate) {
    float a=(gate%16)*2*pi/16;
    Vec center=point(a),forward=tangent(a);
    return dot(previous-center,forward)<0 && dot(current-center,forward)>=0 && length(current-center)<1400 && speed>0;
}
void updateRace(Game& g,Vec previous,Vec current) {
    if (!crosses(previous,current,g.speed,g.nextGate)) return;
    if (++g.nextGate<=16) return;
    g.lastLap=g.lapTime;
    if (g.bestLap<=0 || g.lastLap<g.bestLap) { g.bestLap=g.lastLap;g.notice=1; }
    else g.notice=2;
    g.noticeTime=4;g.lapTime=0;g.nextGate=1;
    if (g.lap==3) {g.finished=true;g.speed=0;g.motion={0,0};g.boosting=false;}
    else ++g.lap;
}
void step(Game& g,float throttle,float brake,float steer,bool handbrake,bool nitro) {
    Vec old=g.position;
    g.offroad=length(old-point(angle(old)))>1190;
    float maxSpeed=g.carType==0?8800:g.carType==1?9100:10500;
    float accel=g.carType==0?2600:g.carType==1?3000:3400;
    g.boosting=nitro && g.boost>.01f && throttle>.1f && g.speed>500 && !g.offroad;
    g.boost=std::clamp(g.boost+(g.boosting?-.22f:.09f)*dt,0.f,1.f);
    if(throttle>.01f) g.speed+=throttle*(g.speed<0?4800:accel*std::clamp(1-g.speed/(maxSpeed*(g.boosting?1.24f:1)),.08f,1.f))*dt;
    if(brake>.01f) g.speed-=brake*(g.speed>0?6200:1500)*dt;
    if(throttle<.01f && brake<.01f) g.speed=approach(g.speed,0,(130+std::abs(g.speed)*.018f)*dt);
    if(handbrake) g.speed=approach(g.speed,0,3200*dt);
    if(g.offroad) g.speed=approach(g.speed,0,1700*dt);
    if(g.boosting) g.speed+=1700*dt;
    g.speed=std::clamp(g.speed,-1800.f,maxSpeed*(g.boosting?1.24f:1));
    g.steering+=(steer-g.steering)*std::min(dt*8,1.f);
    float steeringAngle=g.steering*(30/(1+std::pow(g.speed/1900,2)))*pi/180;
    g.yaw+=g.speed/280*std::tan(steeringAngle)*(handbrake?1.5f:1)*dt;
    Vec target{std::cos(g.yaw)*g.speed,std::sin(g.yaw)*g.speed};
    g.motion=g.motion+(target-g.motion)*std::min(dt*(handbrake?2:g.carType==1?13:9),1.f);
    g.position=g.position+g.motion*dt;
    // Roadside barrier contact, with tangent sliding rather than teleport recovery.
    float a=angle(g.position);Vec center=point(a),offset=g.position-center;
    if(length(offset)>1320) {
        g.position=center+normalized(offset)*1320;
        Vec t=tangent(a);g.motion=t*(dot(g.motion,t)*.6f);g.speed*=.55f;
    }
    updateRace(g,old,g.position);
}
void update(Game& g,float delta,float throttle,float brake,float steer,bool handbrake,bool nitro) {
    if(!std::isfinite(delta) || delta<=0) return;
    delta=std::min(delta,1.f);
    g.noticeTime=std::max(0.f,g.noticeTime-delta);
    if(!g.started || g.finished) return;
    if(g.countdown>0) {g.countdown=std::max(0.f,g.countdown-delta);return;}
    g.raceTime+=delta;g.lapTime+=delta;
    g.accumulator=std::min(g.accumulator+delta,.1f);
    while(g.accumulator+0.0000001f>=dt && !g.finished) {
        step(g,std::clamp(throttle,0.f,1.f),std::clamp(brake,0.f,1.f),std::clamp(steer,-1.f,1.f),handbrake,nitro);
        g.accumulator=std::max(0.f,g.accumulator-dt);
    }
}
}

static vx::Game game;
static float snapshot[24];
extern "C" {
void vx_reset(int preset,float best) {
    game=vx::Game{};game.carType=std::clamp(preset,0,2);
    game.bestLap=std::isfinite(best)?std::clamp(best,0.f,86400.f):0;
}
void vx_start() {if(game.finished) vx_reset(game.carType,game.bestLap);game.started=true;}
void vx_select(int preset) {if(!game.started) game.carType=std::clamp(preset,0,2);}
void vx_recover() {vx::recover(game);game.notice=3;game.noticeTime=2;}
void vx_update(float delta,float throttle,float brake,float steer,int handbrake,int nitro) {
    vx::update(game,delta,throttle,brake,steer,handbrake!=0,nitro!=0);
}
float* vx_state() {
    float values[]={game.position.x,game.position.y,game.yaw,game.speed,game.steering,game.boost,
        game.raceTime,game.lapTime,game.bestLap,game.lastLap,game.countdown,game.noticeTime,
        float(game.lap),float(game.nextGate),float(game.carType),float(game.started),float(game.finished),
        float(game.offroad),float(game.boosting),game.motion.x,game.motion.y,float(game.notice),0,0};
    std::copy(values,values+24,snapshot);return snapshot;
}
// Read-only diagnostics run against isolated game states, never the player's race.
// Each bit corresponds to the native DriveTests regression named in tests/wasm.test.cjs.
int vx_verify() {
    using namespace vx;
    int failures=0;Game g;
    for(int i=0;i<240;++i) step(g,1,0,0,false,false);
    if(!(g.speed>3000 && length(g.position-point(0))>2500)) failures|=1;
    float before=g.speed;for(int i=0;i<60;++i) step(g,0,1,0,false,false);
    if(!(g.speed<before*.5f)) failures|=2;
    g=Game{};for(int i=0;i<60;++i) step(g,0,1,0,false,false);
    if(!(g.speed<0)) failures|=4;
    g=Game{};g.speed=2000;float yaw=g.yaw;
    for(int i=0;i<30;++i) step(g,1,0,1,false,false);
    if(!(std::abs(g.yaw-yaw)>.01745f)) failures|=8;
    g=Game{};g.speed=2000;for(int i=0;i<30;++i) step(g,1,0,0,false,true);
    if(!(g.boost<1 && g.boosting)) failures|=16;
    g=Game{};g.speed=2000;
    auto cross=[&g](int gate,bool forward) {
        Vec p=point(gate*2*pi/16),t=tangent(gate*2*pi/16)*100;
        updateRace(g,p+t*(forward?-1:1),p+t*(forward?1:-1));
    };
    cross(4,true);cross(1,false);
    if(g.nextGate!=1) failures|=32;
    cross(1,true);cross(1,true);
    if(g.nextGate!=2) failures|=64;
    recover(g);
    if(g.nextGate!=2 || length(g.position-point(2*pi/16))>1 || g.speed!=0) failures|=128;
    g=Game{};g.speed=2000;
    for(int lap=0;lap<3;++lap) {g.lapTime=45.f+lap;for(int gate=1;gate<=16;++gate)cross(gate%16,true);}
    if(!g.finished || g.bestLap!=45 || g.speed!=0 || g.lap!=3) failures|=256;
    Game a,b;a.started=b.started=true;a.countdown=b.countdown=0;
    for(int i=0;i<120;++i)update(a,1.f/120,1,0,0,false,false);
    for(int i=0;i<30;++i)update(b,1.f/30,1,0,0,false,false);
    if(length(a.position-b.position)>1 || std::abs(a.speed-b.speed)>1)failures|=512;
    return failures;
}
}
