-- target ��Ԥ�������,��ʾ���뻷��������
project.path = "../build/" .. target
project.name = "matchman"
project.bindir = "../bin"
project.libdir = "../lib"

-----------------------------
-- Antbuster package
-----------------------------
package = newpackage()

package.path = project.path
package.kind = "winexe"
package.name = project.name
package.language = "c++"
package.bindir = "../../bin"

package.config["Debug"].objdir = "./Debug"
package.config["Debug"].target = package.name .. "_d"
package.config["Release"].objdir = "./Release"
package.config["Release"].target = package.name

package.linkoptions ={ "/NODEFAULTLIB:libc" }
package.buildflags = {"no-main", "extra-warnings", "static-runtime", "no-exceptions", "no-rtti" }
package.config["Debug"].links = { "hge", "hgehelp" }
package.config["Release"].links = { "hge", "hgehelp" }
package.includepaths = { "../../include", "../../include/ca", "../../include/hge" }

package.libpaths = { "../../lib", "../../lib/" .. target } 

package.files = {
  matchrecursive("../../include/*.h", "../../src/*.cpp", "../../src/*.h")
}
