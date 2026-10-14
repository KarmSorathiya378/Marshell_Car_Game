"""Idempotently create the authored materials and boot map for the native circuit."""
import unreal
import os

unreal.SystemLibrary.execute_console_command(None, "Interchange.FeatureFlags.Import.FBX 0")
TOOLS = unreal.AssetToolsHelpers.get_asset_tools()
EDIT = unreal.MaterialEditingLibrary
ASSETS = unreal.get_editor_subsystem(unreal.EditorAssetSubsystem)
PALETTE = {
    "Asphalt": ((0.045, 0.055, 0.066), 0.88, 0.0, 0.0),
    "Sand": ((0.34, 0.25, 0.15), 0.95, 0.0, 0.0),
    "Rock": ((0.26, 0.19, 0.14), 0.97, 0.0, 0.0),
    "White": ((0.8, 0.86, 0.88), 0.52, 0.0, 0.0),
    "Red": ((0.65, 0.035, 0.028), 0.34, 0.25, 0.0),
    "Concrete": ((0.38, 0.43, 0.45), 0.85, 0.0, 0.0),
    "Dark": ((0.016, 0.024, 0.03), 0.38, 0.35, 0.0),
    "Cyan": ((0.015, 0.7, 0.52), 0.35, 0.3, 1.8),
    "Trunk": ((0.09, 0.065, 0.04), 0.95, 0.0, 0.0),
    "Leaves": ((0.08, 0.16, 0.095), 0.9, 0.0, 0.0),
    "Paint": ((0.02, 0.48, 0.43), 0.22, 0.65, 0.0),
    "Glass": ((0.012, 0.026, 0.039), 0.13, 0.62, 0.0),
    "Rubber": ((0.014, 0.017, 0.018), 0.94, 0.0, 0.0),
    "Metal": ((0.5, 0.55, 0.58), 0.2, 0.9, 0.0),
    "WhiteGlow": ((0.7, 0.88, 1.0), 0.3, 0.0, 5.0),
    "RedGlow": ((1.0, 0.015, 0.005), 0.3, 0.0, 3.0),
}
for name, (rgb, roughness, metallic, glow) in PALETTE.items():
    path = "/Game/Drive/Materials/M_" + name
    if ASSETS.does_asset_exist(path):
        continue
    mat = TOOLS.create_asset("M_" + name, "/Game/Drive/Materials", unreal.Material, unreal.MaterialFactoryNew())
    color = EDIT.create_material_expression(mat, unreal.MaterialExpressionConstant3Vector)
    color.set_editor_property("constant", unreal.LinearColor(*rgb, 1.0))
    EDIT.connect_material_property(color, "", unreal.MaterialProperty.MP_BASE_COLOR)
    for value, prop in [(roughness, unreal.MaterialProperty.MP_ROUGHNESS), (metallic, unreal.MaterialProperty.MP_METALLIC)]:
        node = EDIT.create_material_expression(mat, unreal.MaterialExpressionConstant)
        node.set_editor_property("r", value)
        EDIT.connect_material_property(node, "", prop)
    if glow:
        node = EDIT.create_material_expression(mat, unreal.MaterialExpressionConstant3Vector)
        node.set_editor_property("constant", unreal.LinearColor(*(v * glow for v in rgb), 1.0))
        EDIT.connect_material_property(node, "", unreal.MaterialProperty.MP_EMISSIVE_COLOR)
    mat.set_editor_property("used_with_instanced_static_meshes", True)
    EDIT.recompile_material(mat)
    ASSETS.save_loaded_asset(mat)
# Import the original authored car geometry. Materials are assigned by the native pawn.
for name in ["SM_CoupeBody", "SM_CoupeGlass", "SM_Ridge"]:
    asset_path = "/Game/Drive/Meshes/" + name
    if ASSETS.does_asset_exist(asset_path):
        continue
    source = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "ArtSource", name + ".fbx"))
    if not os.path.isfile(source):
        raise RuntimeError("Missing authored mesh: " + source)
    task = unreal.AssetImportTask()
    task.set_editor_property("filename", source)
    task.set_editor_property("destination_path", "/Game/Drive/Meshes")
    task.set_editor_property("destination_name", name)
    task.set_editor_property("automated", True)
    task.set_editor_property("replace_existing", True)
    task.set_editor_property("save", True)
    options = unreal.FbxImportUI()
    options.set_editor_property("import_mesh", True)
    options.set_editor_property("import_as_skeletal", False)
    options.set_editor_property("import_materials", False)
    options.set_editor_property("import_textures", False)
    options.set_editor_property("mesh_type_to_import", unreal.FBXImportType.FBXIT_STATIC_MESH)
    task.set_editor_property("options", options)
    TOOLS.import_asset_tasks([task])
    mesh = ASSETS.load_asset(asset_path)
    if mesh:
        unreal.log("DRIVE_MESH_BOUNDS " + name + " " + str(mesh.get_bounding_box()))
    if not ASSETS.does_asset_exist(asset_path):
        raise RuntimeError("Mesh import failed: " + name)
map_path = "/Game/Maps/Sunridge"
if not ASSETS.does_asset_exist(map_path):
    world = unreal.EditorLoadingAndSavingUtils.new_blank_map(False)
    world.get_world_settings().set_editor_property("default_game_mode", unreal.load_class(None, "/Script/VelocityX.DriveGameMode"))
    if not unreal.EditorLoadingAndSavingUtils.save_map(world, map_path):
        raise RuntimeError("Could not save Sunridge map")
unreal.log("DRIVE_ASSETS_READY: materials and Sunridge map saved")
