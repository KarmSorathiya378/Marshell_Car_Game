"""Author the original Velocity X coupe meshes in Blender, then export FBX."""
import bpy, math, os, random
from mathutils import Vector
bpy.ops.object.select_all(action='SELECT')
bpy.ops.object.delete(use_global=False)
out = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', 'ArtSource'))
os.makedirs(out, exist_ok=True)

def hull(name, sections):
    # Eight-point chamfered section, dimensions in meters.
    verts=[]
    for x,w,b,t in sections:
        verts += [(x,-w*.82,b),(x,w*.82,b),(x,w,b+.08),(x,w,t-.07),(x,w*.84,t),(x,-w*.84,t),(x,-w,t-.07),(x,-w,b+.08)]
    faces=[tuple(reversed(range(8)))]
    for k in range(len(sections)-1):
        for j in range(8):
            a=k*8+j; b=k*8+(j+1)%8
            faces.append((a,b,b+8,a+8))
    faces.append(tuple(range((len(sections)-1)*8,len(sections)*8)))
    mesh=bpy.data.meshes.new(name); mesh.from_pydata(verts,[],faces); mesh.update()
    obj=bpy.data.objects.new(name,mesh); bpy.context.collection.objects.link(obj)
    bpy.context.view_layer.objects.active=obj; obj.select_set(True)
    # Recalculate normals before boolean cuts.
    bpy.ops.object.mode_set(mode='EDIT'); bpy.ops.mesh.select_all(action='SELECT'); bpy.ops.mesh.normals_make_consistent(inside=False); bpy.ops.object.mode_set(mode='OBJECT')
    return obj

def export(obj,name):
    bpy.ops.object.select_all(action='DESELECT'); obj.select_set(True); bpy.context.view_layer.objects.active=obj
    bpy.ops.object.mode_set(mode='EDIT'); bpy.ops.mesh.select_all(action='SELECT'); bpy.ops.uv.smart_project(island_margin=.025); bpy.ops.object.mode_set(mode='OBJECT')
    bevel=obj.modifiers.new('Paint edge highlights','BEVEL'); bevel.width=.018; bevel.segments=3
    normal=obj.modifiers.new('Weighted normals','WEIGHTED_NORMAL')
    bpy.ops.export_scene.fbx(filepath=os.path.join(out,name+'.fbx'),use_selection=True,object_types={'MESH'},apply_unit_scale=True,axis_forward='-Y',axis_up='Z',bake_anim=False,add_leaf_bones=False,mesh_smooth_type='FACE')

body=hull('SM_CoupeBody',[(-2.27,.77,-.24,.20),(-2.10,.94,-.25,.34),(-1.40,.97,-.24,.43),(-.60,.88,-.22,.39),(.65,.89,-.22,.32),(1.40,.97,-.24,.34),(2.07,.91,-.23,.20),(2.29,.75,-.16,.12)])
for x in [1.37,-1.41]:
    bpy.ops.mesh.primitive_cylinder_add(vertices=48,radius=.39,depth=2.6,location=(x,0,-.20),rotation=(math.pi/2,0,0))
    cutter=bpy.context.object
    bpy.context.view_layer.objects.active=body
    mod=body.modifiers.new('Wheel arch','BOOLEAN'); mod.operation='DIFFERENCE'; mod.object=cutter
    bpy.ops.object.modifier_apply(modifier=mod.name)
    bpy.data.objects.remove(cutter,do_unlink=True)
export(body,'SM_CoupeBody')
bpy.ops.object.select_all(action='DESELECT')
cabin=hull('SM_CoupeGlass',[(-1.37,.69,.28,.39),(-.66,.67,.29,.85),(.10,.64,.29,.85),(.86,.76,.28,.35)])
export(cabin,'SM_CoupeGlass')
# An original irregular ridge mesh replaces repeated geometric cones.
random.seed(91)
verts=[]; faces=[]; n=20
for iy in range(n+1):
    for ix in range(n+1):
        x=-.5+ix/n; y=-.5+iy/n
        r=math.sqrt((x*1.9)**2+(y*1.8)**2)
        peak=max(0,1-r)**1.25
        ridge=.12*math.sin(x*18+y*7)+.09*math.cos(y*23-x*4)
        z=-.5+peak*(.93+ridge)+random.uniform(-.045,.045)*peak
        verts.append((x+random.uniform(-.012,.012),y+random.uniform(-.012,.012),z))
for iy in range(n):
    for ix in range(n):
        a=iy*(n+1)+ix; b=a+1; c=a+n+1; d=c+1
        faces.extend([(a,b,d),(a,d,c)])
mesh=bpy.data.meshes.new('SM_Ridge'); mesh.from_pydata(verts,[],faces); mesh.update()
mountain=bpy.data.objects.new('SM_Ridge',mesh); bpy.context.collection.objects.link(mountain)
export(mountain,'SM_Ridge')
bpy.ops.wm.save_as_mainfile(filepath=os.path.join(out,'VelocityCoupe.blend'))
print('VELOCITY_MESH_READY')
