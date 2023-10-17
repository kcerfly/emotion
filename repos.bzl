load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

COM_GITHUB_GLOG_GLOG_TAG = ""
COM_GITHUB_GLOG_GLOG_COMMIT = "9051aab54c1aced74b3d76108cade4f3edc2ba0d"

RM_CLIENT_TAG = ""
RM_CLIENT_COMMIT = "4ae8874301ba8765fcdbe2172993f23b6b7b35d9"

TONGOS_CORE_PROTOCOL_TAG = ""
TONGOS_CORE_PROTOCOL_COMMIT = "1c53d0fc0fc295a838f78ac1c0357467cd65bb3a"

DEP_INFO = {
  "com_github_glog_glog": {
    "type": "git_repository",
    "tag": COM_GITHUB_GLOG_GLOG_TAG,
    "commit": COM_GITHUB_GLOG_GLOG_COMMIT,
    "remote": "ssh://git@gitlab.mybigai.ac.cn:2222/TongOS/thirdparty/glog-private.git",
  },
  "rm_client": {
    "type": "git_repository",
    "tag": RM_CLIENT_TAG,
    "commit": RM_CLIENT_COMMIT,
    "remote": "ssh://git@gitlab.mybigai.ac.cn:2222/TongOS/tongos_infrastructure/resource_manager/client_cpp.git",
  },
  "tongos_core_protocol": {
    "type": "git_repository",
    "tag": TONGOS_CORE_PROTOCOL_TAG,
    "commit": TONGOS_CORE_PROTOCOL_COMMIT,
    "remote": "ssh://git@gitlab.mybigai.ac.cn:2222/TongOS/tongos_protocol/tongos_core_protocol.git",
  },
}

def _generic_dependency(name, **kwargs):
    if name not in DEP_INFO:
        fail("Name {} not in dependency info".format(name))
    dep = DEP_INFO[name]
    existing_rules = native.existing_rules()
    if dep["type"] == "git_repository":
        if name not in existing_rules:
            tag = kwargs.get(name + "_tag", "")
            commit = kwargs.get(name + "_commit", "")
            use_commit = False
            use_tag = False
            # 如果用户有设置commit，用用户的commit来获取
            if commit != "":
                use_commit = True
            elif tag != "":
                use_tag = True

            if not use_tag and not use_commit:
                # 如果用户没有设置commit和tag，则用默认commit或者默认tag
                if dep["commit"] != "":
                    commit = dep["commit"]
                    use_commit = True
                elif dep["tag"] != "":
                    tag = dep["tag"]
                    use_tag = True

            if use_commit:
                print("using commit:{} about {}".format(commit,name))
                git_repository(
                    name = name,
                    commit = commit,
                    remote = dep["remote"]
                )
            elif use_tag:
                git_repository(
                    name = name,
                    tag = tag,
                    remote = dep["remote"]
                )
            else:
                fail("Name {} without default tag or commit".format(name))
        else:
            print("Name {} already exist".format(name))

def add_deps_of_eps(**kwargs):
  # gflags needed by glog
  http_archive(
      name = "com_github_gflags_gflags",
      strip_prefix = "gflags-2.2.2",
      sha256 = "19713a36c9f32b33df59d1c79b4958434cb005b5b47dc5400a7a4b078111d9b5",
      url = "https://github.com/gflags/gflags/archive/v2.2.2.zip",
  )

  # Protobuf for Node dependencies
  http_archive(
      name = "rules_proto_grpc",
      sha256 = "bbe4db93499f5c9414926e46f9e35016999a4e9f6e3522482d3760dc61011070",
      strip_prefix = "rules_proto_grpc-4.2.0",
      urls = ["https://github.com/rules-proto-grpc/rules_proto_grpc/archive/4.2.0.tar.gz"],
  )

  http_archive(
    name = "com_github_nlohmann_json",
    strip_prefix = "json-3.11.2",
    sha256 = "d69f9deb6a75e2580465c6c4c5111b89c4dc2fa94e3a85fcd2ffcd9a143d9273",
    url = "https://github.com/nlohmann/json/archive/v3.11.2.tar.gz",
    build_file = "@rm_client//:external/bazel/json.BUILD",
  )

  http_archive(
      name = "com_github_yhirose_httplib",
      strip_prefix = "cpp-httplib-0.13.2",
      sha256 = "b7f64402082af7c42c0d370543cb294d82959ca7cf25b3ee7eb1306732bb27d4",
      url = "https://github.com/yhirose/cpp-httplib/archive/refs/tags/v0.13.2.tar.gz",
      build_file = "@rm_client//:external/bazel/httplib.BUILD",
  )

  _generic_dependency("com_github_glog_glog", **kwargs)
  _generic_dependency("rm_client", **kwargs)
  _generic_dependency("tongos_core_protocol", **kwargs)
