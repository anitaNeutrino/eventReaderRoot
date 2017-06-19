{
  gSystem->Load("${ANITA_UTIL_INSTALL_DIR}/lib/libRootFftwWrapper.so");
  gSystem->Load("${ANITA_UTIL_INSTALL_DIR}/lib/libAnitaEvent.so"); 
  gStyle->SetNumberContours(255);
  gStyle->SetPalette(55);
}
