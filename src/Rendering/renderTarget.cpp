#include"renderTarget.h"
#include"Vulkan/device.h"
#include"Vulkan/image.h"
#include"Vulkan/imageView.h"
namespace mini
{

struct CompareExtent2D
{
    bool operator()(const VkExtent2D& lhs, const VkExtent2D& rhs)const
    {
        return !(lhs.width == rhs.width && lhs.height == rhs.width) && (lhs.width < rhs.width && lhs.height < rhs.width);
    }
};

Attachment::Attachment(VkFormat format, VkSampleCountFlagBits samples, VkImageUsageFlags usage):
    format(format),
    samples(samples),
    usage(usage)
{


}

const RenderTarget::CreateFunc RenderTarget::DEFAULT_CREATE_FUNC = [](Image&& swapchainImage)->std::unique_ptr<RenderTarget> {
    std::vector<Image> images;
    images.push_back(std::move(swapchainImage));

    return std::make_unique<RenderTarget>(std::move(images));
};

RenderTarget::RenderTarget(std::vector<Image>&& images):
    device(images.back().getDevice()),
    images{std::move(images)}
{
    if (this->images.empty()) {
        throw Error("RenderTarget should specify at least 1 image");
    }

    std::set<VkExtent2D, CompareExtent2D> uniqueExtent;

    for (const auto& image : this->images)
    {
        uniqueExtent.insert(image.getExtent());
    }

    if (uniqueExtent.size() != 1)
    {
        throw Error("Extent is not unique");
    }

    extent = *uniqueExtent.begin();

    for (auto& image : this->images)
    {
        views.emplace_back(image);

        attachments.emplace_back(Attachment{ image.getFormat(),image.getSampleCount(),image.getUsage() });
    }
}

RenderTarget::RenderTarget(std::vector<ImageView>&& imageViews)
    :device(imageViews.back().getDevice()), images{},
    views{std::move(imageViews)}
{
    if (this->views.empty()) {
        throw Error("RenderTarget should specify at least 1 image view");
    }

    std::set<VkExtent2D, CompareExtent2D> uniqueExtent;

    for (const auto& view : views)
    {
        uniqueExtent.insert(view.getImage().getExtent());
    }

    if (uniqueExtent.size() != 1)
    {
        throw Error("Extent is not unique");
    }

    extent = *uniqueExtent.begin();

    for (auto& view : views)
    {
        const auto& image = view.getImage();
        attachments.emplace_back(Attachment{ image.getFormat(),image.getSampleCount(),image.getUsage() });
    }

}

const VkExtent2D& RenderTarget::getExtent() const
{
    return extent;
}

const std::vector<ImageView>& RenderTarget::getViews() const
{
    return views;
}

}