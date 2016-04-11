

ug_load_script("ug_util.lua")
InitUG(3, AlgebraType("CPU", 1));

-- TiffImage Operations
tiffFilename = "/Users/anaegel/Pictures/EugenSontak/03_Enhancedlightfix/03_EnhancedlightfixOrig-2.tiff"
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

local numRefs0 = 4  -- 512 = 2**9 -- 16=2**4
for i=1,numRefs0 do
  Refine(promesh_obj, false)
end
SelectAll(promesh_obj)

print(2^numRefs0..", "..2^numRefs0..", "..nz)

-- extrude to 3d
Extrude(promesh_obj, MakeVec(0,0,dz*0.1), 1, true, true)
SaveMesh(promesh_obj, gridName)

-- ug4 operations


--local dom = util.CreateDomain(gridName, 0, requiredSubsets)


local dom = util.CreateDomain(gridName, 0)

local refiner = HangingNodeDomainRefiner(dom);
local maxRefEdgeLen = 512 / 16

print("longEdges="..maxRefEdgeLen)

for i=1,5 do
  MarkAnisotropic_LongEdges(dom, refiner, maxRefEdgeLen)
  maxRefEdgeLen = maxRefEdgeLen * 0.5
  refiner:refine()
  print(dom:domain_info():to_string())
end


-- local dom = util.CreateAndDistributeDomain(gridName, 1, 1)
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