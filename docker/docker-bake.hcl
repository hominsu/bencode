variable "REPO" {
  default = "hominsu"
}

variable "AUTHOR_NAME" {
  default = "hominsu"
}

variable "AUTHOR_EMAIL" {
  default = "hominsu@foxmail.com"
}

variable "ALPINE_VERSION" {
  default = "latest"
}

variable "VERSION" {
  default = ""
}

group "default" {
  targets = [
    "bencode-alpine",
  ]
}

target "bencode-alpine" {
  context    = "."
  dockerfile = "docker/Dockerfile"
  args       = {
    AUTHOR_NAME    = "${AUTHOR_NAME}"
    AUTHOR_EMAIL   = "${AUTHOR_EMAIL}"
    ALPINE_VERSION = "${ALPINE_VERSION}"
    VERSION        = "${VERSION}"
  }
  tags = [
    "${REPO}/bencode:latest",
    notequal("", VERSION) ? "${REPO}/bencode:${VERSION}" : "",
  ]
  platforms = ["linux/amd64", "linux/arm64", "linux/arm"]
}