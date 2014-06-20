// This example shows how you can use the LibHand FileDialog class to
// provide users with a GUI dialog to open or save files.

# include <iostream>

# include "file_dialog.h"

using namespace libhand;
using namespace std;

int main(int argc, char **argv) {
  FileDialog f;

  // The simplest open dialog invocation
  cout << "File name: " << f.Open() << endl;

  // This shows off the main features of the file open/save dialog.
  f.SetTitle("Open a YAML file");

  f.AddExtension(FileExtension("YAML file", 2, ".yml", ".yaml"));
  f.AddExtension(FileExtension("All files", 1, "*"));

  f.SetDefaultExtension(".yml");
  f.SetDefaultName("scene_spec.yml");

  cout << "File name: " << f.Open() << endl;

  // This shows off a save dialog. SetDefaultFile() can be used to
  // suggest a particular file in a particular directory.
  f.SetTitle("Save a YAML file");
  f.SetDefaultFile("/tmp/my_new_spec.yml");

  cout << "Save file name: " << f.Save() << endl;
}
