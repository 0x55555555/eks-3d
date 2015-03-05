RbMake.import_module('../EksCore')

if (!RbMake.module_exists?(:QtCore))
  return
end

RbMake.library(:Eks3D, :Eks) do |l, p|
  l.dependencies << :EksCore << :QtCore

  l.export do |l|
    l.dependencies << :EksCore << :QtCore

    l.when(l.config.platform(:win)) do |l|
      l.cpp.defines << "X_ENABLE_DX_RENDERER"
    end
    
    l.cpp.defines << "X_ENABLE_GL_RENDERER"
  end

  l.when(l.config.platform(:win)) do |l|
    l.cpp.defines << "X_ENABLE_DX_RENDERER"
  end

  l.cpp.defines << "X_ENABLE_GL_RENDERER"
  l.dependencies << :opengl
end 