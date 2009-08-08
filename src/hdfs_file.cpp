#include "hdfs_fs.hpp"
#include "hdfs_file.hpp"

#define exec_and_trap_error(res_type_t, what,err_msg)	\
  res_type_t res = what;\
  if (res < 0){\
    throw hdfs_exception(err_msg);\
  }
    
//-----------------------------------------------
void wrap_hdfs_file::seek(tOffset desidered_pos){
  exec_and_trap_error(int, hdfsSeek(fs_->fs_, file_, desidered_pos),
		      "Cannot seek on " + filename_)
}
//-----------------------------------------------
tOffset wrap_hdfs_file::tell() {
  exec_and_trap_error(int, hdfsTell(fs_->fs_, file_),
		      "Cannot tell on " + filename_);
  return res;
}
//-----------------------------------------------
std::string wrap_hdfs_file::read(tSize length) {
  char* buf = new char[length];
  exec_and_trap_error(tSize, hdfsRead(fs_->fs_, file_, 
				      static_cast<void*>(buf), length),
		      "Cannot read on " + filename_);

  std::string res_s(buf, res);
  delete [] buf;  // fixing #206
  return res_s;
}
//-----------------------------------------------
tSize wrap_hdfs_file::read_chunk(bp::object buffer){
  PyObject *pyo = buffer.ptr();
  void      *buf;
  Py_ssize_t len;
  if (PyObject_AsWriteBuffer(buffer.ptr(), &buf, &len)){
    throw hdfs_exception("Cannot read_chunk on" +
			 filename_);
  }
  exec_and_trap_error(tSize, hdfsRead(fs_->fs_, file_, buf, len),
		      "Cannot read_chunk on " + filename_);
  return res;
}

//-----------------------------------------------
std::string wrap_hdfs_file::pread(tOffset position, tSize length) {
  char* buf = new char[length];
  exec_and_trap_error(tSize,hdfsPread(fs_->fs_, file_, position,
				      static_cast<void*>(buf), length),
		      "Cannot pread on " + filename_);
  std::string res_s(buf, res);
  delete [] buf;  // fixing #206
  return res_s;
}
//-----------------------------------------------
tSize wrap_hdfs_file::pread_chunk(tOffset position, bp::object buffer){
  PyObject *pyo = buffer.ptr();
  void     *buf;
  Py_ssize_t len;
  if (PyObject_AsWriteBuffer(pyo, &buf, &len)){
    throw hdfs_exception("Cannot read_chunk on" + filename_);
  }
  exec_and_trap_error(tSize, hdfsPread(fs_->fs_, file_, position, buf, len),
		      "Cannot read_chunk on " + filename_);
  return res;
}
//-----------------------------------------------
tSize wrap_hdfs_file::write(std::string buffer){
  exec_and_trap_error(tSize, hdfsWrite(fs_->fs_, file_, 
				       static_cast<const void*>(buffer.c_str()),
				       buffer.size()),
		      "Cannot write on " + filename_);
  return res;
}
//-----------------------------------------------
tSize wrap_hdfs_file::write_chunk(bp::object buffer){
  PyObject *pyo = buffer.ptr();
  const void  *buf;
  Py_ssize_t  len;
  if (PyObject_AsReadBuffer(pyo, &buf, &len)) {
    throw hdfs_exception("Cannot write_chunk on" + filename_);
  }
  exec_and_trap_error(tSize, hdfsWrite(fs_->fs_, file_, buf, len),
		      "Cannot write on " + filename_);
  return res;
}
//-----------------------------------------------
int wrap_hdfs_file::available(){
  exec_and_trap_error(int, hdfsAvailable(fs_->fs_, file_),
		      "Cannot get available butes on " + filename_);
  return res;
}
//-----------------------------------------------
void wrap_hdfs_file::_close_helper() {
  if (is_open_){
    int res = hdfsCloseFile(fs_->fs_, file_);
    if (res == -1){
      throw hdfs_exception("Cannot close " + filename_);
    }
    is_open_ = false;
  }
}


//+++++++++++++++++++++++++++++++++++++++++
// Exporting class definitions.
//+++++++++++++++++++++++++++++++++++++++++
void export_hdfs_file() 
{
  using namespace boost::python;
  //--
  class_<wrap_hdfs_file, boost::noncopyable>("hdfs_file", no_init)
    .def("close", &wrap_hdfs_file::close)
    .def("seek", &wrap_hdfs_file::seek)
    .def("tell", &wrap_hdfs_file::tell)
    .def("available", &wrap_hdfs_file::available)
    .def("read", &wrap_hdfs_file::read)
    .def("pread", &wrap_hdfs_file::pread)
    .def("write", &wrap_hdfs_file::write)
    //
    .def("read_chunk", &wrap_hdfs_file::read_chunk)
    .def("pread_chunk", &wrap_hdfs_file::pread_chunk)
    .def("write_chunk", &wrap_hdfs_file::write_chunk)
    ;
}
