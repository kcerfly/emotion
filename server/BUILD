cc_binary(
  name = "server",
  srcs = ["server.cc"],
  linkopts = ["-pthread"],
  deps = [
    "//proto:emotion_proto_lib",
    "//proto:emotion_cpp_grpc_lib",
    "//config:config",
  ],
)