

ug_load_script("ug_util.lua")
InitUG(3, AlgebraType("CPU", 1));

-- TiffImage Operations
tiffFilename = "/Users/anaegel/Pictures/EugenSontak/03_Enhancedlightfix/03_EnhancedlightfixOrig.tif"
gridName = "testImageData.ugx"

tiff = TiffImageDataNumber3d()
tiff:init(tiffFilename);
nx = tiff:get_size_x()
ny = tiff:get_size_y()
nz = tiff:get_size_z()
tiff:set_corner(MakeVec(nx,ny,nz))

dx = 1.0
dy = 1.0
dz = 1.0

print(nx..", "..ny..", "..nz)
-- ProMesh operations
local promesh_obj = Mesh()
CreatePlane(promesh_obj, MakeVec(0,0,0), MakeVec(nx,0,0),  MakeVec(0,ny,0), MakeVec(nx,ny,0), 1, true)
SelectAll(promesh_obj)

numRefs = 6  -- 512 = 2**9
for i=1,numRefs do
  Refine(promesh_obj, false)
end
SelectAll(promesh_obj)

print(2^numRefs..", "..2^numRefs..", "..nz)

-- extrude to 3d
Extrude(promesh_obj, MakeVec(0,0,dz), nz, true, true)
SaveMesh(promesh_obj, gridName)

-- ug4 operations


local dom = util.CreateDomain(gridName, 0, requiredSubsets)
print("\ndomain-info:")
print(dom:domain_info():to_string())

local approxSpace = ApproximationSpace(dom)
approxSpace:add_fct("ufield", "piecewise-constant", 0)
approxSpace:init_levels()
approxSpace:init_top_surface()
approxSpace:print_statistic()


local field = GridFunction(approxSpace)
Interpolate(tiff, field, "ufield")

local vtk=VTKOutput()
vtk:select_element("ufield", "tiffField") 
vtk:print("testImageData", field)