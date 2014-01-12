FN = "version.cpp"

v = IO.read(FN)
ver_major = v.match(/const unsigned VER_MAJOR = (\d+)/)[1]
ver_minor = v.match(/const unsigned VER_MINOR = (\d+)/)[1]
ver_build = 0
v.sub!(/(const unsigned VER_BUILD =) (\d+)/) do
  ver_build = ($2.to_i+1).to_s
  "#{$1} #{ver_build}" 
end
v.sub!(/(const char\* VER_STRING =) .*;/) do
  "#{$1} \"version #{ver_major}.#{ver_minor}.#{ver_build} " +
  "built on #{Time.now.strftime("%Y/%m/%d %H:%M:%S")}\";"
end

File.open(FN, "w") { |f| f.puts v }