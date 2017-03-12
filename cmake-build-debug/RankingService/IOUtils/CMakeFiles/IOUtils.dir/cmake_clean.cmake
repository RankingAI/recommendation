file(REMOVE_RECURSE
  "libIOUtils.pdb"
  "libIOUtils.a"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/IOUtils.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
