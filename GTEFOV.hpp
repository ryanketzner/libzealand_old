#ifndef GTEFOV_hpp
#define GTEFOV_hpp

namespace libzealand
{
template <class GTEPrimative>
class GTEFOV : public VolumeFOV
{
    public:

        GTEFOV(GTEPrimative shape) :
        shape (shape)
        {
        }

        GTEFOV* clone() const override
        {
            return new GTEFOV(*this);
        }

        bool intersects (const AlignedBox3& box) override
        {
            return query(box, shape).intersect;
        }

        bool contains(const Vector3& vector) override
        {
            return gte::InContainer(vector,shape);
        }


        bool contains (const AlignedBox3& box) override
        {
            std::array<Vector3,8> vertices;
            box.GetVertices(vertices);

            for (int i = 0; i < vertices.size(); i++)
            {
                // If any vertex is not in the container,
                // immediately terminate
                if (!gte::InContainer(vertices[i], shape))
                    return false;
            }
            // All vertices are in container
            return true;
        }

    protected:

        GTEPrimative shape;
        gte::TIQuery<Real,AlignedBox3,GTEPrimative> query;
};
}

#endif
