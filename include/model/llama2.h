#ifndef LC_INCLUDE_MODEL_LLAMA_H_
#define LC_INCLUDE_MODEL_LLAMA_H_
#include <map>
#include "model.h"
#include "op/add.h"
#include "op/embedding.h"
#include "op/rope.h"
namespace model {

struct LLamaRawModelData {
  int32_t fd = -1;
  size_t file_size = 0;
  float* data = nullptr;
  float* weight_data = nullptr;

  ~LLamaRawModelData();

  const float* weight(size_t offset) const;

  bool is_weight_valid(size_t peek) const;
};

class LLama2Model : public Model {
 public:
  explicit LLama2Model(std::string token_path, std::string model_path);

  base::Status init(base::DeviceType device_type) override;

  base::Status forward(const std::vector<int>& tokens, int step_pos) override;

  std::vector<int32_t> encode(const std::string& sentence) override;

  std::pair<tensor::Tensor, tensor::Tensor> slice_kv_cache(int32_t layer_idx,
                                                           size_t token_pos) override;

 private:
  void init_mem() override;

  base::Status gen_model_from_file() override;

  void create_rope_layer();

  void create_add_layer();

  void create_mha_layers() override;

  void create_rmsnorm_layers() override;

  void create_embedding_layer() override;

  void create_matmul_layers() override;

  tensor::Tensor& get_buffer(ModelBufferType buffer_idx) override;

  const tensor::Tensor& get_buffer(ModelBufferType buffer_idx) const override;

  base::Status insert_buffer(ModelBufferType buffer_idx, const tensor::Tensor& tensor) override;

 private:
  int32_t kv_dim_ = 0;
  int32_t kv_mul_ = 0;
  int32_t hidden_dim_ = 0;
  int32_t head_size_ = 0;
  int32_t vocab_size_ = 0;

  std::unique_ptr<op::VecAddLayer> add_layer_;
  std::unique_ptr<op::RoPELayer> rope_layer_;
  std::unique_ptr<LlamaModelConfig> config_;
  std::unique_ptr<LLamaRawModelData> raw_model_data_;
};
}  // namespace model

#endif