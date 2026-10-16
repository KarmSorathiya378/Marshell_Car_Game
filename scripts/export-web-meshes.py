"""Run in Blender against the original VelocityCoupe.blend; no assets are edited."""
import bpy, json, os
root = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
out = os.path.join(root, 'VelocityX_Web', 'assets')
os.makedirs(out, exist_ok=True)
depsgraph = bpy.context.evaluated_depsgraph_get()
for name in ['SM_CoupeBody', 'SM_CoupeGlass', 'SM_Ridge']:
    source = bpy.data.objects.get(name)
    if source is None:
        raise RuntimeError('Missing original asset: ' + name)
    if name == 'SM_Ridge':
        # Terrain is instanced 150 times. Keep the original silhouette with a
        # browser LOD rather than rendering every bevel at mountain distance.
        lod = source.modifiers.new('Browser terrain LOD', 'DECIMATE')
        lod.ratio = .25
        depsgraph.update()
    obj = source.evaluated_get(depsgraph)
    mesh = obj.to_mesh()
    mesh.calc_loop_triangles()
    positions, normals = [], []
    for triangle in mesh.loop_triangles:
        for loop_index in triangle.loops:
            vertex = mesh.vertices[mesh.loops[loop_index].vertex_index]
            p = obj.matrix_world @ vertex.co
            n = obj.matrix_world.to_3x3().inverted().transposed() @ mesh.corner_normals[loop_index].vector
            n.normalize()
            positions.extend([round(p.x, 6), round(p.z, 6), round(-p.y, 6)])
            normals.extend([round(n.x, 6), round(n.z, 6), round(-n.y, 6)])
    with open(os.path.join(out, name + '.json'), 'w', encoding='utf-8') as handle:
        json.dump({'name': name, 'positions': positions, 'normals': normals}, handle, separators=(',', ':'))
    obj.to_mesh_clear()
    print('EXPORTED_WEB_MESH', name, len(positions) // 9, 'triangles')
